#include <iostream>
#include <netcdf>

#include "NCFileInfo.h"


namespace TTCore {


NCFileInfo::NCFileInfo(const char* filePath) : ncFilePath(filePath) {}

std::string NCFileInfo::getFilePath() {
    return ncFilePath;
}


void NCFileInfo::checkFileValid() {
    try {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        f.close();
    }
    catch (const std::exception& e) {
        //std::cout << e.what() << std::endl;
        isFileValid = false;
        fileValidInfo = e.what();
        //iiFile->close();
        return;
        //return {false, e.what()};
    }
    isFileValid = true;
}

}
