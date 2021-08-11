#include <vector>
#include "NCFileInfo.h"
#include <netcdf>

using namespace System::Collections::Generic;

namespace myCLI {
    NCFileInfo::NCFileInfo(String ^filePath, String^ latVarName, String^ lonVarName, String^ vorVarName, String^ dumpDirectory) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath), string2Char(latVarName), string2Char(lonVarName), string2Char(vorVarName), string2Char(dumpDirectory))) {
        
    }

    void NCFileInfo::checkFileValid() {
        m_Instance->checkFileValid();
    }

    //void NCFileInfo::openFile() {
    //    m_Instance->openFile();
    //}

    List<String^>^ NCFileInfo::getVarsName() {
        std::vector<std::string> varsName;
        m_Instance->getVarsName(varsName);

        //Debug::WriteLine("varsName::"+varsName);
        //List<String> ^csVarsName = gcnew List<String>;
        //int a = ;
        List<String^> ^sd = gcnew List<String^>(varsName.size());
        //varsNam
        for (auto &f : varsName) {
            sd->Add(gcnew String(f.c_str()));
        }
        return sd;
    }

    void NCFileInfo::startTracking(List<Typhoon ^> ^realTCs) {
        //Debug::WriteLine("sdf");
        std::vector<TTCore::Typhoon> unManagedTC;
        m_Instance->startTracking(unManagedTC);
        Console::WriteLine("unmanagedTC number: {0}",unManagedTC.size());
        ttt(unManagedTC, realTCs);
    }

    void NCFileInfo::startFromStep2() {
        m_Instance->startFromStep2();
    }

    void NCFileInfo::ttt(std::vector<TTCore::Typhoon>& inTC, List<Typhoon^>^ outTC) {
        int n = inTC.size();
        for (int i = 0; i < n; ++i) {
            //Typhoon ^newTC = gcnew Typhoon(inTC[i].ser)
            //outTC.
        }
    }

}
