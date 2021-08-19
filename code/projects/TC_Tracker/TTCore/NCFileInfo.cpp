#include <iostream>
#include <vector>
#include <netcdf>
#include <algorithm>

#include "NCFileInfo.h"
#include "Processor.h"

namespace TTCore {
    NCFileInfo::NCFileInfo(const char *filePath, bool isWrfoutFile, const char* latVName, const char* lonVName, const char* vorVName, const char* dumpDirectory) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), latVarName(latVName), lonVarName(lonVName), vorVarName(vorVName), dumpDir(dumpDirectory) {
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
            //iiFile->close();   // 文件根本没被打开
            return;
            //return {false, e.what()};
        }
        isFileValid = true;
    }

    int NCFileInfo::getZLvDimLenName() {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        auto uVar = f.getVar("U");
        if (uVar.getDimCount() != 4)
            return 0;
        return uVar.getDim(1).getSize();
    }

    void NCFileInfo::getLatLonData(std::vector<float>& latData, std::vector<float>& lonData) {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        UtilFunc::getLatLonData(&f, latVarName, lonVarName, latData, lonData);
    }

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

    void NCFileInfo::startTracking(std::vector<Typhoon> &tcs, bool* isCanceled) {
        
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);

        Processor p(isCanceled, f, isWrfoutFile, latVarName, lonVarName, vorVarName, dumpDir);

        p.recognizeTyphoon();
        if (*isCanceled) return;
        p.dumpStep1();
        std::cout << "finish dump step1." << std::endl;
        p.getRealTC();
        p.dumpStep2();

        p.removeNoise();
        p.dumpStep3();

        p.copyRealTCs(tcs);
    }

    void NCFileInfo::startFromStep2(std::vector<Typhoon>& tcs) {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        //Processor p(f, isWrfoutFile, latVarName, lonVarName, vorVarName, dumpDir);
        //std::string step1FilePath = "E:\\University\\TC_Tracker\\data\\stepFile\\step1\\step1.dat";
        //p.getStep1DataFromFile(step1FilePath);
        //p.getRealTC();
        //p.removeNoise();
        //p.dumpStep3();
        //p.copyRealTCs(tcs);
        
    }

    void NCFileInfo::startFromStep3(std::vector<Typhoon>& tcs) {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        //Processor p(f, isWrfoutFile, latVarName, lonVarName, vorVarName, dumpDir);
        //std::string step1FilePath = "E:\\University\\TC_Tracker\\data\\stepFile\\step2\\step2.dat";
        //p.getStep2DataFromFile(step1FilePath);
        //p.removeNoise();
        //p.dumpStep3();
        //p.copyRealTCs(tcs);
    }

     
}
