#include <iostream>
#include "NCFileInfo.h"


namespace TTCore {


NCFileInfo::NCFileInfo(const char* filePath) : ncFilePath(filePath) {}

std::string NCFileInfo::getFilePath() {
    return ncFilePath;

}


}
