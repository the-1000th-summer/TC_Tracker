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

NCFileInfo::NCFileInfo() {}
NCFileInfo::NCFileInfo(const char* filePath) : ncFilePath(filePath) {}
NCFileInfo::NCFileInfo(const char* filePath, const VarNames &varNames) : ncFilePath(filePath), varNames(varNames) {}
NCFileInfo::NCFileInfo(const char *filePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, double toGridRes, bool noTempFiles, int threadNum, const char *dumpDirectory, const char *resourceBaseDir) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), varNames(varNames), zLevelIndex(zLevelIndex), toGridRes(toGridRes), noTempFiles(noTempFiles), threadNum(threadNum), dumpDir(dumpDirectory), resourceBaseDir(resourceBaseDir) {}

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


void NCFileInfo::startTracking(TCs &tcs, bool* isCanceled, void(*stepPgCallback)(int stepIdx, void*), void(*progressCallback)(double progressValue, void*), void* target) {
    
//    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    stepPgCallback(0, target);
    
    Processor p(isCanceled, ncFilePath, isWrfoutFile, varNames, zLevelIndex, toGridRes, threadNum, dumpDir, resourceBaseDir);
    
    p.recognizeTyphoon(stepPgCallback, progressCallback, target);
    if (*isCanceled) return;
    if (!noTempFiles)
        p.dumpStep1(ncFilePath);
    
    stepPgCallback(4, target);
    progressCallback(-1, target);
    p.getRealTC();
    if (!noTempFiles)
        p.dumpStep2(ncFilePath);
    
    stepPgCallback(5, target);
    p.removeNoise();

    if (!noTempFiles)
        p.dumpStep3(ncFilePath);
    
    //        p.copyRealTCs(tcs);
    p.copyTCs(tcs);
    
    if (!isWrfoutFile)
        p.copyLatLonData(lat_data, lon_data);
}

/// 将结果输出为netCDF文件（标准：CF Convention）
void NCFileInfo::exportFile_nc(TCs &tcs, const std::string &oNcFilePath, const std::string &fullCommand) {
    netCDF::NcFile outFile(oNcFilePath, netCDF::NcFile::replace);
    /// 每个台风的生命长度
    std::vector<int> tcsAge{};
    std::transform(tcs.cbegin(), tcs.cend(), std::back_inserter(tcsAge), [](const Typhoon& tc){return tc.maxVorCells.size();});
    double intpart;          // (无用变量)
    double hourInterval = tcs.getTcInfo().getHourInterval();
    assert(std::modf(hourInterval, &intpart) == 0.0);
    double firstTValue = tcs.getTcInfo().getFirstTValue();
    // 创建维度
    auto stormDimSize = tcs.size();
    auto timeDimSize = *std::max_element(std::begin(tcsAge), std::end(tcsAge));
    auto stormDim = outFile.addDim("storm", stormDimSize);
    auto timeDim = outFile.addDim("date_time", timeDimSize);
    
    // 创建变量
    auto timeVar = outFile.addVar("time", netCDF::NcType::nc_DOUBLE, {stormDim, timeDim});
    auto latVar = outFile.addVar("lat", netCDF::NcType::nc_FLOAT, {stormDim, timeDim});
    auto lonVar = outFile.addVar("lon", netCDF::NcType::nc_FLOAT, {stormDim, timeDim});
    auto serialNoVar = outFile.addVar("serialNo", netCDF::NcType::nc_SHORT, {stormDim, timeDim});
    
    // 准备数据
    auto timeData = std::make_unique<float[]>(stormDimSize * timeDimSize);
    auto latData = std::make_unique<float[]>(stormDimSize * timeDimSize);
    auto lonData = std::make_unique<float[]>(stormDimSize * timeDimSize);
    auto serialNoData = std::make_unique<short[]>(stormDimSize * timeDimSize);
    
    size_t tc_i = 0;
    for (auto const &tc : tcs.getTcs()) {
//        std::iota(timeData.get()+timeDimSize*tc_i, timeData.get()+timeDimSize*tc_i+tc.geoCenters.size(), tc.startTimeIndex);
        double startTI = firstTValue+hourInterval*tc.startTimeIndex - hourInterval;
        std::generate(timeData.get()+timeDimSize*tc_i, timeData.get()+timeDimSize*tc_i+tc.geoCenters.size(), [&startTI, hourInterval]{ return startTI+=hourInterval; });
        std::transform(tc.geoCenters.begin(), tc.geoCenters.end(), latData.get()+timeDimSize*tc_i, [](const std::pair<float, float> &geoCenter){return geoCenter.first;});
        std::transform(tc.geoCenters.begin(), tc.geoCenters.end(), lonData.get()+timeDimSize*tc_i, [](const std::pair<float, float> &geoCenter){return geoCenter.second;});
        std::fill(serialNoData.get()+timeDimSize*tc_i, serialNoData.get()+timeDimSize*tc_i+tc.geoCenters.size(), tc.serialNo);
        // 输入_FillValue
        std::fill(timeData.get()+timeDimSize*tc_i+tc.geoCenters.size(), timeData.get()+timeDimSize*(tc_i+1), -9999000.0);
        std::fill(latData.get()+timeDimSize*tc_i+tc.geoCenters.size(), latData.get()+timeDimSize*(tc_i+1), -9999.0);
        std::fill(lonData.get()+timeDimSize*tc_i+tc.geoCenters.size(), lonData.get()+timeDimSize*(tc_i+1), -9999.0);
        std::fill(serialNoData.get()+timeDimSize*tc_i+tc.geoCenters.size(), serialNoData.get()+timeDimSize*(tc_i+1), -9999);
        ++tc_i;
    }
    
    // 写入变量属性
    timeVar.putAtt("units", tcs.getTimeUnits());
    latVar.putAtt("units", "degrees_north");
    lonVar.putAtt("units", "degrees_east");
    if (tcs.getTcInfo().getTime_noleap()) {
        timeVar.putAtt("calendar", "noleap");
    }
    
    serialNoVar.putAtt("coordinates", "time lon lat");
    timeVar.putAtt("_FillValue", netCDF::NcType::nc_DOUBLE, -9999000.0);
    latVar.putAtt("_FillValue", netCDF::NcType::nc_FLOAT, -9999.0);
    lonVar.putAtt("_FillValue", netCDF::NcType::nc_FLOAT, -9999.0);
    serialNoVar.putAtt("_FillValue", netCDF::NcType::nc_SHORT, -9999);
    // 写入全局属性
    UtilFunc::appendHistoryInfo(outFile, fullCommand);
    outFile.putAtt("featureType", "trajectory");
    UtilFunc::appendThresholdInfo(outFile);
    // 写入数据
    timeVar.putVar(timeData.get());
    latVar.putVar(latData.get());
    lonVar.putVar(lonData.get());
    serialNoVar.putVar(serialNoData.get());
    
    outFile.close();
    
}

}
