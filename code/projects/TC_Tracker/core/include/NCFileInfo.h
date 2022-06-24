#pragma once

#include <string>
#include <vector>

namespace TTCore {

class NCFileInfo {
public:
    std::string ncFilePath;
    bool isFileValid = false;
    std::string fileValidInfo;

    NCFileInfo(const char* filePath);
    std::string getFilePath();
    void checkFileValid();

    void getVarsName(std::vector<std::string>& varsName);
    void getVorDimsName(const std::string& vorVarName, std::vector<std::string>& varsName);
};


}