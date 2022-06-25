#include <iostream>
#include <netcdf>

#include "NCFileInfo.h"


namespace TTCore {


NCFileInfo::NCFileInfo(const char* filePath) : ncFilePath(filePath) {}


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

void NCFileInfo::getVarsName(std::vector<std::string>& varsName) {
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    auto tt = f.getVars();
    for (auto const& imap : tt) {
        std::cout << imap.first << std::endl;
        varsName.push_back(imap.first);
    }
    f.close();
    //std::transform(tt.cbegin(), tt.end(), std::back_inserter(varsName), []( std::pair<std::string, netCDF::NcVar> &a) {return a.first;} );
}
void NCFileInfo::getVorDimsName(const std::string& vorVarName, std::vector<std::string>& varsName) {
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    auto vorVarDims = f.getVar(vorVarName).getDims();
    for (auto const& dim : vorVarDims) {
        varsName.push_back(dim.getName());
    }
}

/// 通过vor变量获取z方向维度名及其长度
/// <param name="zLvDimName"></param>
/// <returns></returns>
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


bool NCFileInfo::checkIsWrfoutFile(std::string& exceptionInfo) {
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
