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

#include "json.hpp"
#include "NCFileInfo.h"
#include "Processor.h"
#include "Typhoon.h"

namespace TTCore {
//NCFileInfo::NCFileInfo(const char* filePath, bool isWrfoutFile, const char* timeVName, const char* latVName, const char* lonVName, const char* vorVName, const char* dumpDirectory) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), timeVarName(timeVName), latVarName(latVName), lonVarName(lonVName), vorVarName(vorVName), dumpDir(dumpDirectory) {}
//NCFileInfo::NCFileInfo(const char* filePath, bool isWrfoutFile, const VarNames &varNames, bool noTempFiles, const char* dumpDirectory) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), varNames(varNames), noTempFiles(noTempFiles), dumpDir(dumpDirectory) {}
//NCFileInfo::NCFileInfo(const char *filePath, bool isWrfoutFile, const char* timeVName, const char* latVName, const char* lonVName, const char* vorVName, int zLevelIndex, const char* dumpDirectory) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), timeVarName(timeVName), latVarName(latVName), lonVarName(lonVName), vorVarName(vorVName), zLevelIndex(zLevelIndex), dumpDir(dumpDirectory) {}
NCFileInfo::NCFileInfo(const char *filePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, bool noTempFiles, int threadNum, const char *dumpDirectory) : ncFilePath(filePath), isWrfoutFile(isWrfoutFile), varNames(varNames), zLevelIndex(zLevelIndex), noTempFiles(noTempFiles), threadNum(threadNum), dumpDir(dumpDirectory) {}
NCFileInfo::NCFileInfo(const char* filePath, const VarNames &varNames) : ncFilePath(filePath), varNames(varNames) {}
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

int NCFileInfo::getZLvDimLenName(std::string& zLvDimName) {
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    auto uVar = f.getVar(isWrfoutFile ? "U" : varNames.vorVarName);
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
    UtilFunc::getLatLonData(&f, varNames.latVarName, varNames.lonVarName, latData, lonData);
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

void NCFileInfo::getVorDimsName(const std::string& vorVarName, std::vector<std::string>& varsName) {
    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    auto vorVarDims = f.getVar(vorVarName).getDims();
    for (auto const& dim : vorVarDims) {
        varsName.push_back(dim.getName());
    }
}

void NCFileInfo::startTracking(TCs &tcs, bool* isCanceled) {
    
//    netCDF::NcFile f(ncFilePath, netCDF::NcFile::read);
    
    Processor p(isCanceled, ncFilePath, isWrfoutFile, varNames, zLevelIndex, threadNum, dumpDir);
    
    p.recognizeTyphoon();
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

void to_json(nlohmann::json& j, const Typhoon& tc) {
    j = nlohmann::json{
        {"mvc", tc.maxVorCells},
        {"gc", tc.geoCenters},
        {"sti", tc.startTimeIndex},
        {"eti", tc.endTimeIndex}
    };
}

void from_json(const nlohmann::json& j, Typhoon& tc) {
    j.at("mvc").get_to(tc.maxVorCells);
    j.at("gc").get_to(tc.geoCenters);
    j.at("sti").get_to(tc.startTimeIndex);
    j.at("eti").get_to(tc.endTimeIndex);
}

void NCFileInfo::exportFile(const std::string& inFilePath, const std::string& outFilePath) {
    std::vector<Typhoon> realTCs{};
    
    std::ifstream ifs(inFilePath, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);
    ia >> realTCs;
    
    nlohmann::json j(realTCs);
    std::ofstream jsonFile(outFilePath);
    //o << std::setw(4) << j << std::endl;
    jsonFile << j;
}
void NCFileInfo::exportFile(const std::string& outFilePath) {
    exportFile((std::filesystem::path(dumpDir) / (std::filesystem::path(ncFilePath).stem().string() + "_step3.dat")).string(), outFilePath);
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
    
    serialNoVar.putAtt("coordinates", "time lon lat");
    timeVar.putAtt("_FillValue", netCDF::NcType::nc_DOUBLE, -9999000.0);
    latVar.putAtt("_FillValue", netCDF::NcType::nc_FLOAT, -9999.0);
    lonVar.putAtt("_FillValue", netCDF::NcType::nc_FLOAT, -9999.0);
    serialNoVar.putAtt("_FillValue", netCDF::NcType::nc_SHORT, -9999);
    // 写入全局属性
    UtilFunc::appendHistoryInfo(outFile, fullCommand);
    outFile.putAtt("featureType", "trajectory");
    outFile.putAtt("vorThresHold", netCDF::NcType::nc_FLOAT, Constants::RECURSION_MIN_ReVOR);
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
    outFile.putAtt("vorThresHold", netCDF::NcType::nc_FLOAT, Constants::RECURSION_MIN_ReVOR);
    
    // 写入数据
    // https://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array
    rowSizeVar.putVar(&tcsAge[0]);         // vector储存的数据是内存连续的，这么做以避免数据拷贝
    timeVar.putVar(timeData.get());
    latVar.putVar(latData.get());
    lonVar.putVar(lonData.get());
    serialNoVar.putVar(serialNoData.get());
    
    outFile.close();
}


void NCFileInfo::getDataFromStep3File(const std::string& inFilePath, std::vector<Typhoon>& tcs) {
    std::ifstream ifs(inFilePath, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);
    ia >> tcs;
}

}
