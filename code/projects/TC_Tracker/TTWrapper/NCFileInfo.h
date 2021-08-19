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
        NCFileInfo(String^ filePath, bool isWrfoutFile, String^ latVarName, String^ lonVarName, String^ vorVarName, String^ dumpDirectory, bool isAsync);
        ~NCFileInfo() { if (isCanceled != nullptr) delete isCanceled; isCanceled = nullptr; }
        !NCFileInfo() { if (isCanceled != nullptr) delete isCanceled; isCanceled = nullptr; }

        void checkFileValid();
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

        property bool isFileValid {
        public:
            bool get() { return m_Instance->isFileValid; }
        private:
            void set(bool value) {}
        }

        property String^ fileValidInfo {
        public:
            String^ get() {
                std::string unmanaged = m_Instance->fileValidInfo;
                //std::cout << m_Instance->isFileValid << "IS VALID?" << std::endl;
                //std::cout << unmanaged << "unmanGED" << std::endl;
                Console::WriteLine(m_Instance->isFileValid+"DEBUG");
                //Debug::WriteLine(m_Instance->fileValidInfo+"DEBUGbvz");
                String ^a = gcnew String(unmanaged.c_str());
                Console::WriteLine(a+"from proper");
                return a;

            }
            void set(String^ value) {}
        }
    private:
        bool *isCanceled;
        //void (*unmanagedFunctionPointer)(bool*);
        void Canceled() { if (isCanceled != nullptr) *isCanceled = true; }
    //    netCDF::NcFile* iiFile = nullptr;
        
    };
}
