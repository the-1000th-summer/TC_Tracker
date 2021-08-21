#pragma once
#include "include/Typhoon.h"
#include <string>
#include <vector>
//#include <netcdf>

namespace TTCore {
    class NCFileInfo {
    public:
        std::string ncFilePath;
        //const char* ncFilePath;
        std::string latVarName;
        std::string lonVarName;
        std::string vorVarName;
        int zLevelIndex = -1;
        std::string dumpDir;
        bool isWrfoutFile = false;
        bool isFileValid = false;
        std::string fileValidInfo;

        NCFileInfo(const char* filePath, bool isWrfoutFile, const char* latVName, const char* lonVName, const char* vorVName, const char* dumpDirectory);
        NCFileInfo(const char *filePath, bool isWrfoutFile, const char *latVName, const char *lonVName, const char *vorVName, int zLevelIndex, const char *dumpDirectory);
        void checkFileValid();
        int getZLvDimLenName(std::string &zLvDimName);
        bool checkIsWrfoutFile(std::string &exceptionInfo);
        //void openFile();
        void getLatLonData(std::vector<float> &latData, std::vector<float>& lonData);

        void getVarsName(std::vector<std::string>& varsName);
        void startTracking(std::vector<Typhoon>& tcs, bool* isCanceled);
        void startFromStep2(std::vector<Typhoon>& tcs);
        void startFromStep3(std::vector<Typhoon>& tcs);
    //private:
    //    netCDF::NcFile *iiFile = nullptr;
    };
}
