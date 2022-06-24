
#include "NCFileInfo.h"


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

String^ NCFileInfo::echoFilePath() {
    std::string filePath = m_Instance->getFilePath();
    return gcnew String(filePath.c_str());
}

}
