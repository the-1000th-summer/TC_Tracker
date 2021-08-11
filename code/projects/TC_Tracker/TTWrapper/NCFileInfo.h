#pragma once
#include <vector>
#include "ManagedObject.h"
#include "../TTCore/Core.h"
//#include <netcdf>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;

namespace myCLI {
    
    public ref class Typhoon {
    public:
        // Typhoon();
        int serialNo;
        List<Tuple<int, int>^>^ maxVorCells = gcnew List<Tuple<int, int>^>();
        int startTimeIndex;
        int endTimeIndex;
    };
    public ref class NCFileInfo: public ManagedObject<TTCore::NCFileInfo> {
    public:
        // 无法在managed class的方法里使用optional paramater
        NCFileInfo(String ^filePath, String ^latVarName, String ^lonVarName, String ^vorVarName, String ^dumpDirectory);
        void checkFileValid();
        //void openFile();
        List<String^>^ getVarsName();
        void ttt(std::vector<TTCore::Typhoon> &inTC, List<Typhoon^> ^outTC);
        void startTracking(List<Typhoon^>^ realTCs);
        void startFromStep2();

        property bool isFileValid {
        public:
            bool get() {
                return m_Instance->isFileValid;
            }
        private:
            void set(bool value) {}
        }

        property String^ fileValidInfo {
        public:
            String^ get() {
                std::string unmanaged = m_Instance->fileValidInfo;
                //std::cout << m_Instance->isFileValid << "IS VALID?" << std::endl;
                //std::cout << unmanaged << "unmanGED" << std::endl;
                Debug::WriteLine(m_Instance->isFileValid+"DEBUG");
                //Debug::WriteLine(m_Instance->fileValidInfo+"DEBUGbvz");
                String ^a = gcnew String(unmanaged.c_str());
                Debug::WriteLine(a+"from proper");
                return a;

            }
            void set(String^ value) {}
        }
    //private:
    //    netCDF::NcFile* iiFile = nullptr;
        
    };
}
