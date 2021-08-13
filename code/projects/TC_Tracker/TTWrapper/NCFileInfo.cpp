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
        copyToManaged(unManagedTC, realTCs);
    }

    void NCFileInfo::startFromStep2(List<Typhoon^>^ outTC) {
        std::vector<TTCore::Typhoon> unManagedTC;
        m_Instance->startFromStep2(unManagedTC);
        Console::WriteLine("step2: unmanagedTC number : {0}", unManagedTC.size());
        copyToManaged(unManagedTC, outTC);
    }

    void NCFileInfo::startFromStep3(List<Typhoon^>^ outTC) {
        std::vector<TTCore::Typhoon> unManagedTC;
        m_Instance->startFromStep3(unManagedTC);
        Console::WriteLine("step3: unmanagedTC number: {0}", unManagedTC.size());
        copyToManaged(unManagedTC, outTC);
    }

    void NCFileInfo::getLatLonData(std::vector<float>& latData, std::vector<float>& lonData) {
        m_Instance->getLatLonData(latData, lonData);
    }

    void NCFileInfo::copyToManaged(std::vector<TTCore::Typhoon>& inTC, List<Typhoon^>^ outTC) {
        std::vector<float> latData{}, lonData{};
        getLatLonData(latData, lonData);

        int n = inTC.size();
        for (int i = 0; i < n; ++i) {
            Typhoon ^newTC = gcnew Typhoon();
            newTC->serialNo = inTC[i].serialNo;
            newTC->startTimeIndex = inTC[i].startTimeIndex;
            newTC->endTimeIndex = inTC[i].endTimeIndex;
            for (const auto& df : inTC[i].maxVorCells) {
                newTC->maxVorCells->Add(gcnew Tuple<float,float>{latData[df.first], lonData[df.second]});
            }
            for (const auto& geoCenter : inTC[i].geoCenters) {
                newTC->geoCenters->Add(gcnew Tuple<float, float>{geoCenter.first, geoCenter.second});
            }
            
            outTC->Add(newTC);
        }
    }

}
