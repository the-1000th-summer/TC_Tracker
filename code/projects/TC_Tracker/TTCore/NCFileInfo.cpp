#include <iostream>
#include <vector>
#include <netcdf>
#include <algorithm>

#include "NCFileInfo.h"
#include "Processor.h"

namespace TTCore {
    NCFileInfo::NCFileInfo(const char *filePath) : ncFilePath(filePath) {
        //checkFileValid();
    }

    void NCFileInfo::checkFileValid() {
        try {
            netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
            //iiFile = &f;
            f.close();
        }
        catch (const std::exception& e) {
            //std::cout << e.what() << std::endl;
            isFileValid = false;
            fileValidInfo = e.what();
            //iiFile->close();   // 文件根本没被打开
            return;
            //return {false, e.what()};
        }
        isFileValid = true;
        //return {true, ""};
    }

    //void NCFileInfo::openFile() {
    //    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    //    iiFile = &f;
    //    std::cout << "sdf" << std::endl;
    //}

    void NCFileInfo::getVarsName(std::vector<std::string> &varsName) {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        auto tt = f.getVars();
        for (auto const &imap : tt) {
            std::cout << imap.first << std::endl;
            varsName.push_back(imap.first);
        }
        f.close();
        //std::transform(tt.cbegin(), tt.end(), std::back_inserter(varsName), []( std::pair<std::string, netCDF::NcVar> &a) {return a.first;} );
    }

    void NCFileInfo::startTracking() {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        Processor p(f);
        p.recognizeTyphoon();
        p.getRealTC();
        //p.removeNoise();
    }
}
