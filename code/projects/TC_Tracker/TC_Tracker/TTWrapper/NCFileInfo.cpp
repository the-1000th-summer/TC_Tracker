#include <vector>
#include "NCFileInfo.h"

using namespace System::Collections::Generic;

namespace myCLI {

TCInfo::TCInfo(String^ timeUnits, bool time_noleap, double timeInterval, double firstTValue) : timeUnits(timeUnits), time_noleap(time_noleap), timeInterval(timeInterval), firstTValue(firstTValue) {
}

TCs::TCs(List<Typhoon^>^ tcs, TCInfo^ tcInfo) : tcs(tcs), tcInfo(tcInfo) {
}

NCFileInfo::NCFileInfo() : ManagedObject(new TTCore::NCFileInfo()) {

}

NCFileInfo::NCFileInfo(String^ filePath) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath))) {

}

NCFileInfo::NCFileInfo(String^ filePath, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath), VarNames(string2Char(time), string2Char(lat), string2Char(lon), string2Char(vor), string2Char(u), string2Char(v), dataIsVor))) {
    
}

NCFileInfo::NCFileInfo(String^ filePath, bool isWrfoutFile, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor, int zLevelIndex, int threadNum, double toGridRes, String^ tempFileDir) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath), isWrfoutFile, VarNames(string2Char(time), string2Char(lat), string2Char(lon), string2Char(vor), string2Char(u), string2Char(v), dataIsVor), zLevelIndex, toGridRes, true, threadNum, string2Char(tempFileDir), "")) {
    shouldCancel = new bool;
    *shouldCancel = false;
}

bool NCFileInfo::checkFileValid(String^% fileValidInfo) {
    m_Instance->checkFileValid();
    std::string unmanaged = m_Instance->fileValidInfo;
    Console::WriteLine(m_Instance->isFileValid + "DEBUG");
    //Debug::WriteLine(m_Instance->fileValidInfo+"DEBUGbvz");
    fileValidInfo = gcnew String(unmanaged.c_str());

    return m_Instance->isFileValid;
}

bool NCFileInfo::checkIsWrfoutFile(String^% exceptionInfo) {
    std::string eInfo = "";
    bool isWrfoutFile = m_Instance->checkIfIsWrfoutFile(eInfo);
    exceptionInfo = gcnew String(eInfo.c_str());
    return isWrfoutFile;
}

List<String^>^ NCFileInfo::getVarsName() {
    auto varsName = m_Instance->getVarsName();

    //Debug::WriteLine("varsName::"+varsName);
    //List<String> ^csVarsName = gcnew List<String>;
    //int a = ;
    List<String^>^ varsNameList = gcnew List<String^>(varsName.size());
    //varsNam
    for (auto& varName : varsName) {
        varsNameList->Add(gcnew String(varName.c_str()));
    }
    return varsNameList;
}

List<String^>^ NCFileInfo::getVorDimsName(String^ vorVarName) {
    auto vorDimVec = m_Instance->getVorDimsName(string2Char(vorVarName));
    List<String^>^ vorDimList = gcnew List<String^>(vorDimVec.size());
    for (auto& vorDimName : vorDimVec) {
        vorDimList->Add(gcnew String(vorDimName.c_str()));
    }
    return vorDimList;
}

int NCFileInfo::getZLvDimLenName(String^% zLvDimName) {
    std::string dimName = "";
    int zLvDimLen = m_Instance->getZLvDimLenName(dimName);
    zLvDimName = gcnew String(dimName.c_str());
    return zLvDimLen;
}

TCs^ NCFileInfo::startTracking(StepPgCallback^ stepPgCallback, ProgressCallback^ progressCallback, CancellationToken cancelToken) {
    //bool isCanceled = false;
    CancellationTokenRegistration reg = cancelToken.Register(gcnew Action(this, &NCFileInfo::Canceled));

    GCHandle gch1 = GCHandle::Alloc(stepPgCallback);
    GCHandle gch2 = GCHandle::Alloc(progressCallback);

    TTCore::CppCallBack stepPgCallbackPt = (TTCore::CppCallBack)Marshal::GetFunctionPointerForDelegate(stepPgCallback).ToPointer();
    TTCore::CppCallBack2 progressCallbackPt = (TTCore::CppCallBack2)Marshal::GetFunctionPointerForDelegate(progressCallback).ToPointer();

    TTCore::TCs tcs;
    m_Instance->startTracking(tcs, stepPgCallbackPt, progressCallbackPt, nullptr, shouldCancel);

    //copyToManaged(tcs, realTCs);

    gch1.Free();
    gch2.Free();

    return copyToManaged(tcs);
}

TCs^ NCFileInfo::copyToManaged(TTCore::TCs& inTC) {
    auto realTCs = inTC.getTcs();
    int tcSize = inTC.size();
    List<Typhoon^>^ outTC = gcnew List<Typhoon^>();

    for (int i = 0; i < tcSize; ++i) {
        Typhoon^ newTC = gcnew Typhoon();
        newTC->serialNo = realTCs[i].serialNo;
        newTC->startTimeIndex = realTCs[i].startTimeIndex;
        newTC->endTimeIndex = realTCs[i].endTimeIndex;
        for (const auto& maxVorCell : realTCs[i].maxVorCells) {
            // newTC->maxVorCells->Add(gcnew Tuple<float,float>{latData[maxVorCell.first], lonData[maxVorCell.second]});
            newTC->maxVorCells->Add(gcnew Tuple<float, float>{maxVorCell.first, maxVorCell.second});
        }
        for (const auto& geoCenter : realTCs[i].geoCenters) {
            newTC->geoCenters->Add(gcnew Tuple<float, float>{geoCenter.first, geoCenter.second});
        }

        outTC->Add(newTC);
    }

    TCInfo^ newTCInfo = gcnew TCInfo(
        gcnew String(inTC.getTimeUnits().c_str()),
        inTC.getTcInfo().getTime_noleap(),
        inTC.getTimeInterval(),
        inTC.getTcInfo().getFirstTValue()
    );

    return gcnew TCs(outTC, newTCInfo);
}

void NCFileInfo::exportFile_json(TCs^ tcs, String^ oNcFilePath) {
    TTCore::TCs tcs_cpp = toCppTCs(tcs);
    m_Instance->exportFile_json(tcs_cpp, string2Char(oNcFilePath));
}

void NCFileInfo::exportFile_proto3(TCs^ tcs, String^ oNcFilePath) {
    TTCore::TCs tcs_cpp = toCppTCs(tcs);
    m_Instance->exportFile_proto3(tcs_cpp, string2Char(oNcFilePath));
}

void NCFileInfo::exportFile_nc(TCs^ tcs, String^ oNcFilePath, String^ fullCommand) {
    TTCore::TCs tcs_cpp = toCppTCs(tcs);
    m_Instance->exportFile_nc(tcs_cpp, string2Char(oNcFilePath), string2Char(fullCommand));
}

void NCFileInfo::exportFile_nc_compact(TCs^ tcs, String^ oNcFilePath, String^ fullCommand) {
    TTCore::TCs tcs_cpp = toCppTCs(tcs);
    m_Instance->exportFile_nc_compact(tcs_cpp, string2Char(oNcFilePath), string2Char(fullCommand));
}

TTCore::TCs NCFileInfo::toCppTCs(TCs^ tcs) {
    std::vector<TTCore::Typhoon> realTCs;

    for each (Typhoon^ realTC in tcs->tcs) {
        std::vector<std::pair<int, int>> maxVorCells_cpp;
        std::vector<std::pair<float, float>> geoCenters_cpp;
        for each (Tuple<float, float>^ maxVorCell in realTC->maxVorCells) {
            maxVorCells_cpp.push_back({maxVorCell->Item1, maxVorCell->Item2});
        }
        for each (Tuple<float, float>^ geoCenter in realTC->geoCenters) {
            geoCenters_cpp.push_back({geoCenter->Item1, geoCenter->Item2});
        }

        realTCs.push_back(TTCore::Typhoon{realTC->serialNo, maxVorCells_cpp, geoCenters_cpp, realTC->startTimeIndex, realTC->endTimeIndex, {}});
    }
    
    auto tcInfo = TTCore::TCInfo(string2Char(tcs->tcInfo->timeUnits), tcs->tcInfo->time_noleap, tcs->tcInfo->timeInterval, tcs->tcInfo->firstTValue);
    auto tcs_cpp = TTCore::TCs(realTCs, tcInfo);

    return tcs_cpp;
}

}
