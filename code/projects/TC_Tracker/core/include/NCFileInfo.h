#pragma once

#include <string>
#include <vector>
#include <netcdf>

#include "Typhoon.h"
#include "TCInfo.h"
#include "Processor.h"

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
    
    NCFileInfo();
    NCFileInfo(const char* filePath);
    NCFileInfo(const char* filePath, const VarNames &varNames);
    NCFileInfo(const char *filePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, double toGridRes, bool noTempFiles, int threadNum, const char *dumpDirectory, const char *resourceBaseDir);
    
    void checkFileValid();
    int getZLvDimLenName(std::string& zLvDimName);
    bool checkIfIsWrfoutFile(std::string& exceptionInfo);

    std::vector<std::string> getVarsName();
    std::vector<std::string> getVorDimsName(const std::string& vorVarName);
    
    void startTracking(TCs &tcs, bool* isCanceled, void(*stepPgCallback)(int stepIdx, void*), void(*progressCallback)(double progressValue, void*), void* target);
    
    void exportFile_json(TCs &tcs, const std::string oFilePath);
    void exportFile_proto3(TCs &tcs, const std::string oFilePath);
    void exportFile_nc(TCs &tcs, const std::string &oNcFilePath, const std::string &fullCommand);
    void exportFile_nc_compact(const TCs &tcs, const std::string &oNcFilePath, const std::string &fullCommand);
    
private:
    std::string resourceBaseDir;
    std::vector<float> lat_data, lon_data;
    
    double toGridRes = 0.0;
};


}
