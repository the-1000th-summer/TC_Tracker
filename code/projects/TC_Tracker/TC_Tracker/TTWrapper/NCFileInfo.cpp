#include <vector>
#include "NCFileInfo.h"

using namespace System::Collections::Generic;

namespace myCLI {

NCFileInfo::NCFileInfo(String^ filePath) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath))) {

}

NCFileInfo::NCFileInfo(String^ filePath, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath), VarNames(string2Char(time), string2Char(lat), string2Char(lon), string2Char(vor), string2Char(u), string2Char(v), dataIsVor))) {
    
}

NCFileInfo::NCFileInfo(String^ filePath, bool isWrfoutFile, String^ time, String^ lat, String^ lon, String^ vor, String^ u, String^ v, bool dataIsVor, int zLevelIndex, double toGridRes, String^ tempFileDir) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath), isWrfoutFile, VarNames(string2Char(time), string2Char(lat), string2Char(lon), string2Char(vor), string2Char(u), string2Char(v), dataIsVor), zLevelIndex, toGridRes, true, 1, string2Char(tempFileDir), "")) {
    
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

/// 此函数获取z方向维度的名字和其长度
/// @param[inout] z方向维度名字
/// @return z方向维度长度
int NCFileInfo::getZLvDimLenName(String^% zLvDimName) {
    std::string dimName = "";
    int zLvDimLen = m_Instance->getZLvDimLenName(dimName);
    zLvDimName = gcnew String(dimName.c_str());
    return zLvDimLen;
}

void NCFileInfo::startTracking(List<Typhoon^>^ realTCs, CancellationToken cancelToken, StepPgCallback^ stepPgCallback, ProgressCallback^ progressCallback) {
    GCHandle gch1 = GCHandle::Alloc(stepPgCallback);
    GCHandle gch2 = GCHandle::Alloc(progressCallback);

    TTCore::CppCallBack stepPgCallbackPt = (TTCore::CppCallBack)Marshal::GetFunctionPointerForDelegate(stepPgCallback).ToPointer();
    TTCore::CppCallBack2 progressCallbackPt = (TTCore::CppCallBack2)Marshal::GetFunctionPointerForDelegate(progressCallback).ToPointer();

    TTCore::TCs tcs;
    bool isCanceled = false;
    m_Instance->startTracking(tcs, &isCanceled, stepPgCallbackPt, progressCallbackPt, nullptr);

    copyToManaged(tcs, realTCs);

    gch1.Free();
    gch2.Free();
}

void NCFileInfo::copyToManaged(TTCore::TCs& inTC, List<Typhoon^>^ outTC) {
    auto tcs = inTC.getTcs();

    int tcSize = inTC.size();
    for (int i = 0; i < tcSize; ++i) {
        Typhoon^ newTC = gcnew Typhoon();
        newTC->serialNo = tcs[i].serialNo;
        newTC->startTimeIndex = tcs[i].startTimeIndex;
        newTC->endTimeIndex = tcs[i].endTimeIndex;
        for (const auto& maxVorCell : tcs[i].maxVorCells) {
            // newTC->maxVorCells->Add(gcnew Tuple<float,float>{latData[maxVorCell.first], lonData[maxVorCell.second]});
            newTC->maxVorCells->Add(gcnew Tuple<float, float>{maxVorCell.first, maxVorCell.second});
        }
        for (const auto& geoCenter : tcs[i].geoCenters) {
            newTC->geoCenters->Add(gcnew Tuple<float, float>{geoCenter.first, geoCenter.second});
        }

        outTC->Add(newTC);
    }
}


}
