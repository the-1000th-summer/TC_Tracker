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
//    std::string timeVarName;
//    std::string latVarName;
//    std::string lonVarName;
//    std::string vorVarName;
    int zLevelIndex = -1;
    std::string dumpDir;
    bool isWrfoutFile = false;
    bool isFileValid = false;
    std::string fileValidInfo;
    
//    NCFileInfo(const char* filePath, bool isWrfoutFile, const char *timeVName, const char* latVName, const char* lonVName, const char* vorVName, const char* dumpDirectory);
    NCFileInfo(const char* filePath, bool isWrfoutFile, const VarNames &varNames, const char* dumpDirectory);
//    NCFileInfo(const char *filePath, bool isWrfoutFile, const char* timeVName, const char *latVName, const char *lonVName, const char *vorVName, int zLevelIndex, const char *dumpDirectory);
    NCFileInfo(const char *filePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, const char *dumpDirectory);
    NCFileInfo(const char* filePath);
    void checkFileValid();
    int getZLvDimLenName(std::string &zLvDimName);
    bool checkIsWrfoutFile(std::string &exceptionInfo);
    //void openFile();
    void getLatLonData(std::vector<float> &latData, std::vector<float>& lonData);
    
    void getVarsName(std::vector<std::string>& varsName);
    void getVorDimsName(const std::string &vorVarName, std::vector<std::string>& varsName);
    void startTracking(TCs &tcs, bool* isCanceled);
    void startFromStep2(std::vector<Typhoon>& tcs);
    void startFromStep3(std::vector<Typhoon>& tcs);
    
    void exportFile(const std::string &inFilePath, const std::string &outFilePath);
    void exportFile(const std::string &outFilePath);
    void exportFile_nc(TCs &tcs, const std::string &oNcFilePath);
    void exportFile_nc_compact(const TCs &tcs, const std::string &oNcFilePath);
    
    void getDataFromStep3File(const std::string& inFilePath, std::vector<Typhoon>& tcs);
private:
    void appendHistoryInfo(netCDF::NcFile &ncFile);
    //    netCDF::NcFile *iiFile = nullptr;
};
}
