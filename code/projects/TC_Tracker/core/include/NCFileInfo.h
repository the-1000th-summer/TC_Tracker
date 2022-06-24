#pragma once

#include <string>

namespace TTCore {

class NCFileInfo {
public:
    std::string ncFilePath;
    bool isFileValid = false;
    std::string fileValidInfo;

    NCFileInfo(const char* filePath);
    std::string getFilePath();
    void checkFileValid();
};


}