#include <iostream>
#include <fstream>
#include <vector>
#include <netcdf>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <chrono>

#include "json.hpp"
#include "NCFileInfo.h"
#include "Processor.h"
#include "Typhoon.h"
#include "TCsP.pb.h"

namespace TTCore {


NCFileInfo::NCFileInfo(const char* filePath) : ncFilePath(filePath) {}
NCFileInfo::NCFileInfo(const char* filePath, const VarNames &varNames) : ncFilePath(filePath), varNames(varNames) {}
NCFileInfo::NCFileInfo(const char *filePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, bool noTempFiles, int threadNum, const char *dumpDirectory, const char *resourceBaseDir) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), varNames(varNames), zLevelIndex(zLevelIndex), noTempFiles(noTempFiles), threadNum(threadNum), dumpDir(dumpDirectory), resourceBaseDir(resourceBaseDir) {}

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

std::vector<std::string> NCFileInfo::getVarsName() {
    std::vector<std::string> varsName;
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    auto tt = f.getVars();
    for (auto const& imap : tt) {
        std::cout << imap.first << std::endl;
        varsName.push_back(imap.first);
    }
    f.close();
    return varsName;
    //std::transform(tt.cbegin(), tt.end(), std::back_inserter(varsName), []( std::pair<std::string, netCDF::NcVar> &a) {return a.first;} );
}
std::vector<std::string> NCFileInfo::getVorDimsName(const std::string& vorVarName) {
    std::vector<std::string> varsName;
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    auto vorVarDims = f.getVar(vorVarName).getDims();
    for (auto const& dim : vorVarDims) {
        varsName.push_back(dim.getName());
    }
    return varsName;
}

int NCFileInfo::getZLvDimLenName(std::string& zLvDimName) {
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    auto theVar = f.getVar(varNames.theVarName());
    if (theVar.getDimCount() != 4)
        return 0;
    zLvDimName = theVar.getDim(1).getName();
    std::cout << "ZName: " << zLvDimName << std::endl;
    return static_cast<int>(theVar.getDim(1).getSize());
//    return 1;
}


bool NCFileInfo::checkIfIsWrfoutFile(std::string& exceptionInfo) {
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    for (const std::string& dimName : { "Time", "south_north", "west_east" }) {
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
    for (const std::string& varName : { "U", "V", "MAPFAC_U", "MAPFAC_V", "MAPFAC_M", "XLAT_U", "XLAT_V", "XLONG_U", "XLONG_V" }) {
        if (f.getVar(varName).isNull()) {
            exceptionInfo = "No variable: " + varName;
            return false;
        }
    }
    return true;
}


void NCFileInfo::startTracking(TCs &tcs, bool* isCanceled, void(*progressCallback)(void*), void(*resultCallback)(bool result, void* target), void* target) {
    
//    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    
    Processor p(isCanceled, ncFilePath, isWrfoutFile, varNames, zLevelIndex, threadNum, dumpDir, resourceBaseDir);
    
    p.recognizeTyphoon(progressCallback, target);
    if (*isCanceled) return;
    if (!noTempFiles)
        p.dumpStep1(ncFilePath);
    
    p.getRealTC();
    if (!noTempFiles)
        p.dumpStep2(ncFilePath);
    
    p.removeNoise();

    if (!noTempFiles)
        p.dumpStep3(ncFilePath);
    
    //        p.copyRealTCs(tcs);
    p.copyTCs(tcs);
    
    if (!isWrfoutFile)
        p.copyLatLonData(lat_data, lon_data);
}

}
