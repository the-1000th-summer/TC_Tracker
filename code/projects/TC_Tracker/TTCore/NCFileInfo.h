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
        std::string dumpDir;
        bool isWrfoutFile = false;
        bool isFileValid = false;
        std::string fileValidInfo;

        NCFileInfo(const char *filePath, bool isWrfoutFile, const char *latVName, const char *lonVName, const char *vorVName, const char *dumpDirectory);
        void checkFileValid();
        //void openFile();
        void getLatLonData(std::vector<float> &latData, std::vector<float>& lonData);

        void getVarsName(std::vector<std::string>& varsName);
        void startTracking(std::vector<Typhoon>& tcs);
        void startFromStep2(std::vector<Typhoon>& tcs);
        void startFromStep3(std::vector<Typhoon>& tcs);
    //private:
    //    netCDF::NcFile *iiFile = nullptr;
    };
}
