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

String^ NCFileInfo::echoFilePath() {
    std::string filePath = m_Instance->getFilePath();
    return gcnew String(filePath.c_str());
}

}
