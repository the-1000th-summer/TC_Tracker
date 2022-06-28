#pragma once

#include <string>
#include <vector>

namespace TTCore {

class NCFileInfo {
public:
    std::string ncFilePath;
    //const char* ncFilePath;
    //VarNames varNames;
    //int zLevelIndex = -1;
    //bool noTempFiles = false;
    //int threadNum = 1;
    std::string dumpDir;
    bool isWrfoutFile = false;
    bool isFileValid = false;
    std::string fileValidInfo;

    NCFileInfo(const char* filePath);
    void checkFileValid();
    int getZLvDimLenName(std::string& zLvDimName);
    bool checkIsWrfoutFile(std::string& exceptionInfo);

    std::vector<std::string> getVarsName();
    std::vector<std::string> getVorDimsName(const std::string& vorVarName);
};


}
