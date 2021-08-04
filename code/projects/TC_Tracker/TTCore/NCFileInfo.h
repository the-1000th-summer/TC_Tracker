#pragma once
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
        bool isFileValid = false;
        std::string fileValidInfo;

        NCFileInfo(const char *filePath, const char *latVName, const char *lonVName, const char *vorVName);
        void checkFileValid();
        //void openFile();
        void getVarsName(std::vector<std::string>& varsName);
        void startTracking();
    //private:
    //    netCDF::NcFile *iiFile = nullptr;
    };
}
