#pragma once
#include "ManagedObject.h"
#include "../TTCore/Core.h"
//#include <netcdf>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;

namespace myCLI {
    public ref class NCFileInfo: public ManagedObject<TTCore::NCFileInfo> {
    public:
        // 无法在managed class的方法里使用optional paramater
        NCFileInfo(String ^filePath, String ^latVarName, String ^lonVarName, String ^vorVarName);
        void checkFileValid();
        //void openFile();
        List<String^>^ getVarsName();
        void startTracking();


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
