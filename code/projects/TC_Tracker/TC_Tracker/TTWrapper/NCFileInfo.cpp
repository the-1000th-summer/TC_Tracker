
#include "NCFileInfo.h"


namespace myCLI {

NCFileInfo::NCFileInfo(String^ filePath) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath))) {

}

String^ NCFileInfo::echoFilePath() {
    std::string filePath = m_Instance->getFilePath();
    return gcnew String(filePath.c_str());
}

}
