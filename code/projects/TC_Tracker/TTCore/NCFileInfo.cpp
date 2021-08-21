#include <iostream>
#include <vector>
#include <netcdf>
#include <algorithm>

#include "NCFileInfo.h"
#include "Processor.h"

namespace TTCore {
    NCFileInfo::NCFileInfo(const char* filePath, bool isWrfoutFile, const char* latVName, const char* lonVName, const char* vorVName, const char* dumpDirectory) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), latVarName(latVName), lonVarName(lonVName), vorVarName(vorVName), dumpDir(dumpDirectory) {
    }
    NCFileInfo::NCFileInfo(const char *filePath, bool isWrfoutFile, const char* latVName, const char* lonVName, const char* vorVName, int zLevelIndex, const char* dumpDirectory) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), latVarName(latVName), lonVarName(lonVName), vorVarName(vorVName), zLevelIndex(zLevelIndex), dumpDir(dumpDirectory) {
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

    int NCFileInfo::getZLvDimLenName(std::string& zLvDimName) {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        auto uVar = f.getVar(isWrfoutFile ? "U" : vorVarName);
        if (uVar.getDimCount() != 4)
            return 0;
        zLvDimName = uVar.getDim(1).getName();
        std::cout << "ZName: " << zLvDimName << std::endl;
        return uVar.getDim(1).getSize();
    }

    bool NCFileInfo::checkIsWrfoutFile(std::string& exceptionInfo) {
        netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
        for (const std::string &dimName : {"Time", "south_north", "west_east"}) {
            if (f.getDim(dimName).isNull()) {
                exceptionInfo = "No dimension: " + dimName;
                return false;
            } 
        }
        for (const std::string& globalAttName : { "DX", "DY" }) {
            if (f.getAtt(globalAttName).isNull()) {
                exceptionInfo = "No global attribute: " + globalAttName;
                return false;
            }
        }
        for (const std::string& varName : { "U", "V", "MAPFAC_U", "MAPFAC_V", "MAPFAC_M"}) {
            if (f.getVar(varName).isNull()) {
                exceptionInfo = "No variable: " + varName;
                return false;
            }
        }
        return true;
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

        Processor p(isCanceled, f, isWrfoutFile, latVarName, lonVarName, vorVarName, zLevelIndex, dumpDir);

        p.recognizeTyphoon();
        if (*isCanceled) return;
        p.dumpStep1(ncFilePath);
        std::cout << "finish dump step1." << std::endl;
        p.getRealTC();
        p.dumpStep2(ncFilePath);

        p.removeNoise();
        p.dumpStep3(ncFilePath);

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
