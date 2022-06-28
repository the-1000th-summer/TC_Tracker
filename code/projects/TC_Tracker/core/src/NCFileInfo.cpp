#include <iostream>
#include <netcdf>

#include "NCFileInfo.h"


namespace TTCore {


NCFileInfo::NCFileInfo(const char* filePath) : ncFilePath(filePath) {}
NCFileInfo::NCFileInfo(const char* filePath, const VarNames &varNames) : ncFilePath(filePath), varNames(varNames) {}

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
    //netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    //auto uVar = f.getVar(isWrfoutFile ? "U" : varNames.vorVarName);
    //if (uVar.getDimCount() != 4)
        //return 0;
    //zLvDimName = uVar.getDim(1).getName();
    //std::cout << "ZName: " << zLvDimName << std::endl;
    //return uVar.getDim(1).getSize();
    return 1;
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
    for (const std::string& varName : { "U", "V", "MAPFAC_U", "MAPFAC_V", "MAPFAC_M" }) {
        if (f.getVar(varName).isNull()) {
            exceptionInfo = "No variable: " + varName;
            return false;
        }
    }
    return true;
}

}
