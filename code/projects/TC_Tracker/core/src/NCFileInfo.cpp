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


void NCFileInfo::startTracking(TCs &tcs, bool* isCanceled, void(*stepPgCallback)(int stepIdx, void*), void(*progressCallback)(double progressValue, void*), void* target, bool* shouldCancel) {
    
//    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    stepPgCallback(0, target);
    
    Processor p(shouldCancel, ncFilePath, isWrfoutFile, varNames, zLevelIndex, toGridRes, threadNum, dumpDir, resourceBaseDir);
    
    p.recognizeTyphoon(stepPgCallback, progressCallback, target);
    if (*shouldCancel) return;
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

void to_json(nlohmann::json& j, const Typhoon& tc) {
    j = nlohmann::json{
        {"maxVorCells", tc.maxVorCells},
        {"geoCenters", tc.geoCenters},
        {"startTimeIndex", tc.startTimeIndex},
        {"endTimeIndex", tc.endTimeIndex}
    };
}
void from_json(const nlohmann::json& j, Typhoon& tc) {
    j.at("maxVorCells").get_to(tc.maxVorCells);
    j.at("geoCenters").get_to(tc.geoCenters);
    j.at("startTimeIndex").get_to(tc.startTimeIndex);
    j.at("endTimeIndex").get_to(tc.endTimeIndex);
}

void to_json(nlohmann::json& j, const TCInfo& tcInfo) {
    j = nlohmann::json{
        {"timeUnits", tcInfo.getTimeUnits()},
        {"time_noleap", tcInfo.getTime_noleap()},
        {"timeInterval", tcInfo.getTimeInterval()},
        {"firstTValue", tcInfo.getFirstTValue()}
    };
}
//void from_json(const nlohmann::json& j, TCInfo& tcInfo) {
//    j.at("timeUnits").get_to(tcInfo.getTimeUnits());
//}

void to_json(nlohmann::json& j, const TCs& tcs) {
    j = nlohmann::json{
        {"tcs", tcs.getTcs()},
        {"tcInfo", tcs.getTcInfo()}
    };
}
//void from_json(const nlohmann::json& j, TCs& tcs) {
//
//}

void NCFileInfo::exportFile_json(TCs &tcs, const std::string oFilePath) {
    nlohmann::json tcs_jsonObj(tcs);
    std::ofstream jsonFile(oFilePath);
    jsonFile << tcs_jsonObj;
}

void NCFileInfo::exportFile_proto3(TCs &tcs, const std::string oFilePath) {
    TCsP tcsP;
    for (const Typhoon &tc : tcs.getTcs()) {
        auto tc_ptr = tcsP.add_typhoons();
        
        for (const std::pair<int, int> &maxVorCell : tc.maxVorCells) {
            auto maxVorCell_ptr = tc_ptr->add_maxvorcells();
            maxVorCell_ptr->set_latindex(maxVorCell.first);
            maxVorCell_ptr->set_lonindex(maxVorCell.second);
        }
        for (const std::pair<float, float> &geoCenter : tc.geoCenters) {
            auto geoCenter_ptr = tc_ptr->add_geocenters();
            geoCenter_ptr->set_lat(geoCenter.first);
            geoCenter_ptr->set_lon(geoCenter.second);
        }
        tc_ptr->set_starttimeindex(tc.startTimeIndex);
        tc_ptr->set_endtimeindex(tc.endTimeIndex);
        for (const std::vector<std::pair<int, int>> &vorCellsIndex : tc.vorsCellsIndex) {
            auto vorCellsIndex_ptr = tc_ptr->add_vorscellsindex();
            for (const std::pair<int, int> &cellIndex : vorCellsIndex) {
                auto cellIndex_ptr = vorCellsIndex_ptr->add_vorcellsindex();
                cellIndex_ptr->set_latindex(cellIndex.first);
                cellIndex_ptr->set_lonindex(cellIndex.second);
            }
        }
    }
    auto tcInfo = tcs.getTcInfo();
    tcsP.set_timeunits(tcInfo.getTimeUnits());
    tcsP.set_time_noleap(tcInfo.getTime_noleap());
    tcsP.set_firsttimevalue(tcInfo.getFirstTValue());
    tcsP.set_timehourinterval(tcInfo.getHourInterval());
    
    if (!lat_data.empty()) {        // lat and lon is not wrf file
        for (auto i : lat_data) { tcsP.add_lats(i); }
        for (auto i : lon_data) { tcsP.add_lons(i); }
    }
    
    std::fstream output(oFilePath, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!tcsP.SerializeToOstream(&output)) {
        std::cout << "Failed to write proto3 file." << std::endl;
        exit(-1);
    }
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

void NCFileInfo::exportFile_nc_compact(const TCs &tcs, const std::string &oNcFilePath, const std::string &fullCommand) {
    netCDF::NcFile outFile(oNcFilePath, netCDF::NcFile::replace);
    std::vector<int> tcsAge{};
    std::transform(tcs.cbegin(), tcs.cend(), std::back_inserter(tcsAge), [](const Typhoon& tc){return tc.maxVorCells.size();});
    
    
    // 创建维度
    auto stormDimSize = tcs.size();
    int timeDimSize = std::accumulate(tcsAge.begin(), tcsAge.end(), 0);
    
    auto stormDim = outFile.addDim("storm", stormDimSize);
    auto timeDim = outFile.addDim("date_time", timeDimSize);
    
    // 创建变量
    auto rowSizeVar = outFile.addVar("rowSize", netCDF::NcType::nc_INT, {stormDim});
    auto timeVar = outFile.addVar("time", netCDF::NcType::nc_DOUBLE, {timeDim});
    auto latVar = outFile.addVar("lat", netCDF::NcType::nc_FLOAT, {timeDim});
    auto lonVar = outFile.addVar("lon", netCDF::NcType::nc_FLOAT, {timeDim});
    auto serialNoVar = outFile.addVar("serialNo", netCDF::NcType::nc_SHORT, {timeDim});
    
    // 准备数据
    auto timeData = std::make_unique<float[]>(timeDimSize);
    auto latData = std::make_unique<float[]>(timeDimSize);
    auto lonData = std::make_unique<float[]>(timeDimSize);
    auto serialNoData = std::make_unique<short[]>(timeDimSize);
    
    size_t tc_i = 0, pastTimeLen = 0;
    for (auto const &tc : tcs.getTcs()) {
        std::iota(timeData.get()+pastTimeLen, timeData.get()+pastTimeLen+tc.geoCenters.size(), tc.startTimeIndex);
        std::transform(tc.geoCenters.begin(), tc.geoCenters.end(), latData.get()+pastTimeLen, [](const std::pair<float, float> &geoCenter){return geoCenter.first;});
        std::transform(tc.geoCenters.begin(), tc.geoCenters.end(), lonData.get()+pastTimeLen, [](const std::pair<float, float> &geoCenter){return geoCenter.second;});
        std::fill(serialNoData.get()+pastTimeLen, serialNoData.get()+pastTimeLen+tc.geoCenters.size(), tc.serialNo);
        pastTimeLen += tc.geoCenters.size();
        ++tc_i;
    }
    
    // 写入变量属性
    timeVar.putAtt("units", tcs.getTimeUnits());
    latVar.putAtt("units", "degrees_north");
    lonVar.putAtt("units", "degrees_east");
    
    serialNoVar.putAtt("coordinates", "time lon lat");
    // 写入全局属性
    UtilFunc::appendHistoryInfo(outFile, fullCommand);
    outFile.putAtt("featureType", "trajectory");
    UtilFunc::appendThresholdInfo(outFile);
    
    // 写入数据
    // https://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array
    rowSizeVar.putVar(&tcsAge[0]);         // vector储存的数据是内存连续的，这么做以避免数据拷贝
    timeVar.putVar(timeData.get());
    latVar.putVar(latData.get());
    lonVar.putVar(lonData.get());
    serialNoVar.putVar(serialNoData.get());
    
    outFile.close();
}

}
