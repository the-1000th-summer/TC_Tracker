#pragma once

#include <string>

namespace TTCore {

class NCFileInfo {
public:
    std::string ncFilePath;

    NCFileInfo(const char* filePath);
    std::string getFilePath();
};


}