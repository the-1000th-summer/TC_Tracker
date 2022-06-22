
#include "NCFileInfo.h"


namespace myCLI {

NCFileInfo::NCFileInfo(String^ filePath) : ManagedObject(new TTCore::NCFileInfo(string2Char(filePath))) {

}

void NCFileInfo::echoFilePath() {
    std::string filePath = m_Instance->getFilePath();

}

}
