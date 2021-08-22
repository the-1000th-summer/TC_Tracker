#include <vector>
#include "NCFileInfo.h"
#include <netcdf>

using namespace System::Collections::Generic;


namespace myCLI {
    NCFileInfo::NCFileInfo(String ^filePath, bool isWrfoutFile, String^ latVarName, String^ lonVarName, String^ vorVarName, String^ dumpDirectory) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath), isWrfoutFile, string2Char(latVarName), string2Char(lonVarName), string2Char(vorVarName), string2Char(dumpDirectory))) {

    }
    NCFileInfo::NCFileInfo(String^ filePath, bool isWrfoutFile, String^ latVarName, String^ lonVarName, String^ vorVarName, int zLevelIndex, String^ dumpDirectory, bool isAsync) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath), isWrfoutFile, string2Char(latVarName), string2Char(lonVarName), string2Char(vorVarName), zLevelIndex, string2Char(dumpDirectory))) {
        isCanceled = new bool;
    }

    bool NCFileInfo::checkFileValid(String^ %fileValidInfo) {
        m_Instance->checkFileValid();
        std::string unmanaged = m_Instance->fileValidInfo;
        Console::WriteLine(m_Instance->isFileValid + "DEBUG");
        //Debug::WriteLine(m_Instance->fileValidInfo+"DEBUGbvz");
        fileValidInfo = gcnew String(unmanaged.c_str());

        return m_Instance->isFileValid;
    }

    int NCFileInfo::getZLvDimLenName(String^% zLvDimName) {
        std::string dimName = "";
        int zLvDimLen = m_Instance->getZLvDimLenName(dimName);
        zLvDimName = gcnew String(dimName.c_str());
        return zLvDimLen;
    }

    bool NCFileInfo::checkIsWrfoutFile(String^% exceptionInfo) {
        std::string eInfo = "";
        bool isWrfoutFile = m_Instance->checkIsWrfoutFile(eInfo);
        exceptionInfo = gcnew String(eInfo.c_str());
        return isWrfoutFile;
    }

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

    void NCFileInfo::startTracking(List<Typhoon ^> ^realTCs, CancellationToken cancelToken) {
        //Debug::WriteLine("sdf");
        *isCanceled = false;
        CancellationTokenRegistration reg = cancelToken.Register(
            gcnew Action(this, &NCFileInfo::Canceled));

        std::vector<TTCore::Typhoon> unManagedTC;
        m_Instance->startTracking(unManagedTC, isCanceled);
        if (*isCanceled) {
            Console::WriteLine("msg from wrapper: cancelled!!!");
            return;
        }
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
        //std::vector<float> latData{}, lonData{};
        //getLatLonData(latData, lonData);
        

        int n = inTC.size();
        for (int i = 0; i < n; ++i) {
            Typhoon ^newTC = gcnew Typhoon();
            newTC->serialNo = inTC[i].serialNo;
            newTC->startTimeIndex = inTC[i].startTimeIndex;
            newTC->endTimeIndex = inTC[i].endTimeIndex;
            for (const auto& maxVorCell : inTC[i].maxVorCells) {
                // newTC->maxVorCells->Add(gcnew Tuple<float,float>{latData[maxVorCell.first], lonData[maxVorCell.second]});
                newTC->maxVorCells->Add(gcnew Tuple<float, float>{maxVorCell.first, maxVorCell.second});
            }
            for (const auto& geoCenter : inTC[i].geoCenters) {
                newTC->geoCenters->Add(gcnew Tuple<float, float>{geoCenter.first, geoCenter.second});
            }
            
            outTC->Add(newTC);
        }
    }

    void NCFileInfo::exportFile(String^ inFilePath, String^ outFilePath) {
        m_Instance->exportFile(string2Char(inFilePath), string2Char(outFilePath));
    }
    void NCFileInfo::exportFile(String ^outFilePath) {
        m_Instance->exportFile(string2Char(outFilePath));
    }

    void NCFileInfo::getDataFromStep3File(String^ inFilePath, List<Typhoon^>^ outTCs) {
        std::vector<TTCore::Typhoon> inTCs{};
        m_Instance->getDataFromStep3File(string2Char(inFilePath), inTCs);
        copyToManaged(inTCs, outTCs);
    }

}
