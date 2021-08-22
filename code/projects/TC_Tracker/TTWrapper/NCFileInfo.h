#pragma once
#include <vector>
#include "ManagedObject.h"
#include "../TTCore/Core.h"
//#include <netcdf>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

namespace myCLI {
    
    public ref class Typhoon {
    public:
        // Typhoon();
        int serialNo;
        List<Tuple<float, float>^>^ maxVorCells = gcnew List<Tuple<float, float>^>();
        List<Tuple<float, float>^>^ geoCenters = gcnew List<Tuple<float, float>^>();
        int startTimeIndex;
        int endTimeIndex;
    };
    public ref class NCFileInfo: public ManagedObject<TTCore::NCFileInfo> {
    public:
        // 无法在managed class的方法里使用optional paramater
        NCFileInfo(String ^filePath, bool isWrfoutFile, String ^latVarName, String ^lonVarName, String ^vorVarName, String ^dumpDirectory);
        NCFileInfo(String^ filePath, bool isWrfoutFile, String^ latVarName, String^ lonVarName, String^ vorVarName, int zLevelIndex, String^ dumpDirectory, bool isAsync);
        ~NCFileInfo() { if (isCanceled != nullptr) delete isCanceled; isCanceled = nullptr; }
        !NCFileInfo() { if (isCanceled != nullptr) delete isCanceled; isCanceled = nullptr; }

        bool checkFileValid(String^ %fileValidInfo);
        int getZLvDimLenName(String^ %zLvDimName);
        bool checkIsWrfoutFile(String^ %exceptionInfo);
        //void openFile();
        List<String^>^ getVarsName();
        void getLatLonData(std::vector<float> &latData, std::vector<float>& lonData);

        void copyToManaged(std::vector<TTCore::Typhoon> &inTC, List<Typhoon^> ^outTC);
        void startTracking(List<Typhoon^>^ realTCs, CancellationToken cancelToken);
        void startFromStep2(List<Typhoon^>^ outTC);
        void startFromStep3(List<Typhoon^>^ outTC);
        
        //property bool isWrfoutFile {
        //public:
        //    bool get() { return m_Instance->isWrfoutFile; }
        //private:
        //    void set(bool value) {}
        //}
        void exportFile(String ^inFilePath, String^ outFilePath);
        void exportFile(String^ outFilePath);
        void getDataFromStep3File(String^ inFilePath, List<Typhoon^>^ outTCs);
    private:
        bool *isCanceled;
        //void (*unmanagedFunctionPointer)(bool*);
        void Canceled() { if (isCanceled != nullptr) *isCanceled = true; }
    //    netCDF::NcFile* iiFile = nullptr;
        
    };
}
