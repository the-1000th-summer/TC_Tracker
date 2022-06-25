#include <vector>
#include "NCFileInfo.h"

using namespace System::Collections::Generic;

namespace myCLI {

NCFileInfo::NCFileInfo(String^ filePath) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath))) {

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
    List<String^>^ varsNameList = gcnew List<String^>(varsName.size());
    //varsNam
    for (auto& varName : varsName) {
        varsNameList->Add(gcnew String(varName.c_str()));
    }
    return varsNameList;
}

void NCFileInfo::getVorDimsName(String^ vorVarName, List<String^>^ vorDimList) {
    std::vector<std::string> vorDimVec;
    m_Instance->getVorDimsName(string2Char(vorVarName), vorDimVec);
    for (auto& vorDimName : vorDimVec) {
        vorDimList->Add(gcnew String(vorDimName.c_str()));
    }
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
