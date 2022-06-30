#pragma once

#include <string>
#include <vector>
#include <netcdf>

#include "Typhoon.h"
#include "TCInfo.h"

namespace TTCore {

class NCFileInfo {
public:
    std::string ncFilePath;
    //const char* ncFilePath;
    VarNames varNames;
    int zLevelIndex = -1;
    bool noTempFiles = false;
    int threadNum = 1;
    std::string dumpDir;
    bool isWrfoutFile = false;
    bool isFileValid = false;
    std::string fileValidInfo;

    NCFileInfo(const char* filePath);
    NCFileInfo(const char* filePath, const VarNames &varNames);
    NCFileInfo(const char *filePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, bool noTempFiles, int threadNum, const char *dumpDirectory, const char *resourceBaseDir);
    
    void checkFileValid();
    int getZLvDimLenName(const std::string &theVarName, std::string& zLvDimName);
    bool checkIfIsWrfoutFile(std::string& exceptionInfo);

    std::vector<std::string> getVarsName();
    std::vector<std::string> getVorDimsName(const std::string& vorVarName);
    
    void startTracking(TCs &tcs, bool* isCanceled);
    
private:
    std::string resourceBaseDir;
    std::vector<float> lat_data, lon_data;
};


}
