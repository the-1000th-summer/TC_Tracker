#pragma once
#include <string>
#include <vector>
//#include <netcdf>

namespace TTCore {
    class NCFileInfo {
    public:
        std::string ncFilePath;
        //const char* ncFilePath;
        //std::string vorVarName;
        //std::string latVarName;
        //std::string lonVarName;
        bool isFileValid = false;
        std::string fileValidInfo;

        NCFileInfo(const char *filePath);
        void checkFileValid();
        //void openFile();
        void getVarsName(std::vector<std::string>& varsName);
        void startTracking();
    //private:
    //    netCDF::NcFile *iiFile = nullptr;
    };
}
