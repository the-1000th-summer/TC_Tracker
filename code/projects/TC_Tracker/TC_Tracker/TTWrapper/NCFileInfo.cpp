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


}
