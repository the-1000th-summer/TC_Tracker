
#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <ostream>
#include <set>
#include <utility>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <filesystem>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <omp.h>
#include <nlohmann/json.hpp>

#include "multiArray.h"
#include "Processor.h"
#include "Utils.h"
#include "Typhoon.h"
#include "VortexesDumper.h"

#ifndef DISABLE_PROTOBUF
#include "TCsP.pb.h"
#endif

#include "linint2.h"
#include "uv2vr_cfd.h"
#include "rcm2rgrid.h"
// for MS compiler
#ifndef M_PI
    #define M_PI 3.14159265358979323846264338327950288
#endif
namespace TTCore {

Processor::Processor(std::atomic_bool* shouldCancel, const std::string &iFilePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, double toGridRes, int threadNum, const std::string& dumpDirectory, const std::string &resourceBaseDir) : shouldCancel(shouldCancel), iFilePath(iFilePath), isWrfoutFile(isWrfoutFile), varNames(varNames), zLevelIndex(zLevelIndex), toGridRes(toGridRes), threadNum(threadNum), dumpDir(dumpDirectory), resourceBaseDir(resourceBaseDir), iiFile{std::make_unique<netCDF::NcFile>(iFilePath, netCDF::NcFile::read)}, tcInfo(getTCInfo()) {

    threadNum ? threadNum : omp_get_max_threads();
    getDimLength();
    allVortexes = std::vector<std::vector<TC1Time>>(timeLength);
    if (isWrfoutFile) {
        latArr2D = TwoDArray(latGridNum, lonGridNum);
        lonArr2D = TwoDArray(latGridNum, lonGridNum);
        UtilFunc::getLatLonData2d(iiFile.get(), latGridNum, lonGridNum, latArr2D.get(), lonArr2D.get());
    } else {
        latArr = std::make_unique<float[]>(latGridNum);
        lonArr = std::make_unique<float[]>(lonGridNum);
        UtilFunc::getLatLonData(iiFile.get(), varNames.latVarName, varNames.lonVarName, latArr.get(), lonArr.get());
    }

    // std::cout << a << std::endl;
    // iFile = netCDF::NcFile("/mnt/e/University/TC_Tracker/data/Vorticity_JRA-55_hourly.nc", netCDF::NcFile::read)
    checkDirAndCreate(Constants::STEP_FILE_NAME);
}

//Processor::Processor(netCDF::NcFile& iFile, bool isWrfoutFile, const std::string& dumpDirectory) {
//    if (!isWrfoutFile) { throw std::runtime_error("should use another method!"); }
//    iiFile = &iFile;
//    isWrfoutFile = true;
//}

Processor::~Processor() {
    iiFile->close();
    iiFile = nullptr;
}

TCInfo Processor::getTCInfo() {
    // time units
    std::string timeUnits;
    auto timeVar = iiFile->getVar(varNames.timeVarName);
    timeVar.getAtt("units").getValues(timeUnits);
    // time calendar
    bool time_noleap;

    if (!UtilFunc::checkIfHasVarAtt(timeVar, "calendar")) {
        time_noleap = false;
    } else {
        std::string calendarStr;
        timeVar.getAtt("calendar").getValues(calendarStr);
        time_noleap = (calendarStr == "noleap");
    }
    double firstTValue;
    timeVar.getVar({0}, {1}, &firstTValue);
    return TCInfo(timeUnits, time_noleap, UtilFunc::getTimeInterval(timeVar), firstTValue);
    //        vortexes = Vortexes(timeUnits, UtilFunc::getTimeInterval(timeVar));
//    return Vortexes(timeUnits, UtilFunc::getTimeInterval(timeVar));
}

/// 此方法找出文件的各维度的长度
void Processor::getDimLength() {
    if (!isWrfoutFile) {
        auto theVar = iiFile->getVar(varNames.theVarName());
        auto theVarDimLen = theVar.getDimCount();
        timeLength = theVar.getDim(0).getSize();
        latGridNum = theVar.getDim(theVarDimLen - 2).getSize();
        lonGridNum = theVar.getDim(theVarDimLen - 1).getSize();
    } else {
        timeLength = iiFile->getDim("Time").getSize();
        latGridNum = iiFile->getDim("south_north").getSize();
        lonGridNum = iiFile->getDim("west_east").getSize();
    }

}

/// 此方法计算wrfout文件的相对涡度
/// @param[in] inFile nc文件
/// @param[out] rv 存储相对涡度的变量
void Processor::calcRelativeVorField(netCDF::NcFile *inFile, ThreeDArray& rv) {
    size_t nt = inFile->getDim("Time").getSize();
    size_t ny = inFile->getDim("south_north").getSize();
    size_t nx = inFile->getDim("west_east").getSize();
    float dx, dy;
    inFile->getAtt("DX").getValues(&dx);
    inFile->getAtt("DY").getValues(&dy);
    auto msfm = ThreeDArray(nt, ny, nx);
    inFile->getVar("MAPFAC_M").getVar(msfm.get());
    auto u = ThreeDArray(nt, ny, nx + 1), msfu = ThreeDArray(nt, ny, nx + 1);
    auto v = ThreeDArray(nt, ny + 1, nx), msfv = ThreeDArray(nt, ny + 1, nx);
    if (zLevelIndex == -1) {
        inFile->getVar("U").getVar({ 0,0,0 }, { nt,ny,nx + 1 }, u.get());
        inFile->getVar("V").getVar({ 0,0,0 }, { nt,ny + 1,nx }, v.get());
    } else {
        inFile->getVar("U").getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { nt,1,ny,nx + 1 }, u.get());
        inFile->getVar("V").getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { nt,1,ny + 1,nx }, v.get());
    }

    inFile->getVar("MAPFAC_U").getVar(msfu.get());
    inFile->getVar("MAPFAC_V").getVar(msfv.get());

    for (int k = 0; k < nt; ++k) {
        for (int j = 0; j < ny; ++j) {
            // jp1: 1,2,3,4...,ny-2,ny-1,ny-1
            int jp1 = std::min(j+1, static_cast<int>(ny-1));
            // jm1: 0,0,1,2...,ny-4,ny-3,ny-2
            int jm1 = std::max(j-1, 0);
            for (int i = 0; i < nx; ++i) {
                // ip1: 1,2,3,4,...,nx-2,nx-1,nx-1
                int ip1 = std::min(i+1, static_cast<int>(nx-1));
                // im1: 0,0,1,2,...,nx-4,nx-3,nx-2
                int im1 = std::max(i-1, 0);

                float dsx = (ip1 - im1) * dx;     // (1,2,2,2,...,2,2,1)*dx
                float dsy = (jp1 - jm1) * dy;     // (1,2,2,2,...,2,2,1)*dy
                float mm = msfm(k, j, i) * msfm(k, j, i);

                auto dudy = 0.5 * (u(k, jp1, i) / msfu(k, jp1, i) +
                                   u(k, jp1, i + 1) / msfu(k, jp1, i + 1) -
                                   u(k, jm1, i) / msfu(k, jm1, i) -
                                   u(k, jm1, i + 1) / msfu(k, jm1, i + 1)) / dsy * mm;

                auto dvdx = 0.5 * (v(k, j, ip1) / msfv(k, j, ip1) +
                                   v(k, j + 1, ip1) / msfv(k, j + 1, ip1) -
                                   v(k, j, im1) / msfv(k, j, im1) -
                                   v(k, j + 1, im1) / msfv(k, j + 1, im1)) / dsx * mm;

                rv(k, j, i) = dvdx - dudy;
            }
        }
    }
}

/// 第一步：找出有台风的日期，记录日期与台风信息
void Processor::recognizeTyphoon(void(*stepPgCallback)(int stepIdx, void*), void(*progressCallback)(double progressValue, void*), void* target) {
    std::cout << "step 1 (recognize_typhoon): successfully read data，start tracking..." << std::endl;

    /// 记录前一个时次的台风数目
//    int TCNum_prevTime = 0;
    //constexpr int startYear = 1979, endYear = 2018;

    /// startYear的1月1日0时时次在文件中的index
//    int startIndexInFile = 0;
    /// endYear的12月31日0时时次在文件中的index
    //        int endIndexInFile = 58436;
    allVorsCellsIndex = std::vector<std::vector<std::unordered_set<std::pair<int, int>, pair_hash>>>(timeLength);
//    auto vorField = ThreeDArray(timeLength, latGridNum, lonGridNum);
//    std::unique_ptr<ThreeDArray> vorField;
    auto vorField = ThreeDArray();

    if (isWrfoutFile) {
        if (toGridRes > 0) {

            auto minLatD = latArr2D.max(0).second / toGridRes;
            auto maxLatD = latArr2D.min(latGridNum-1).second / toGridRes;
            auto minLonD = lonArr2D.maxInColumn(0) / toGridRes;
            auto maxLonD = lonArr2D.minInColumn(lonGridNum-1) / toGridRes;

            float minLatDIntegral, maxLatDIntegral;
            float minLonDIntegral, maxLonDIntegral;
            modf(minLatD, &minLatDIntegral);
            modf(maxLatD, &maxLatDIntegral);
            modf(minLonD, &minLonDIntegral);
            modf(maxLonD, &maxLonDIntegral);

            const float minLat_rged = (minLatD <= 0) ? minLatDIntegral*toGridRes : (minLatDIntegral+1)*toGridRes;
            const float maxLat_rged = (maxLatD < 0) ? (maxLatDIntegral-1)*toGridRes : maxLatDIntegral*toGridRes;
            // 不考虑有lon负值的情况
            const float minLon_rged = (minLonDIntegral + 1) * toGridRes;
            const float maxLon_rged = maxLonDIntegral * toGridRes;

            std::vector<float> lat_rged;
            std::vector<float> lon_rged;
            float nowLat = minLat_rged;
            while (true) {
                lat_rged.push_back(nowLat);
                nowLat += toGridRes;
                if (nowLat > maxLat_rged)
                    break;
            }
            float nowLon = minLon_rged;
            while (true) {
                lon_rged.push_back(nowLon);
                nowLon += toGridRes;
                if (nowLon > maxLon_rged)
                    break;
            }

            stepPgCallback(1, target);     // start regrid

            auto u_unstged = ThreeDArray(timeLength,latGridNum,lonGridNum);
            auto v_unstged = ThreeDArray(timeLength,latGridNum,lonGridNum);
            unstaggerU(iiFile.get(), u_unstged, v_unstged);

            auto u_regularGrid = ThreeDArray(timeLength, lat_rged.size(), lon_rged.size());
            auto v_regularGrid = ThreeDArray(timeLength, lat_rged.size(), lon_rged.size());
            int ier = 0;

            int completed_count = 0;
            unsigned long itsPerCheck = timeLength / 20;

#           pragma omp parallel for num_threads(threadNum)
            for (int time_i = 0; time_i < timeLength; ++time_i) {
                if (shouldCancel->load()) { continue; }

                NCL_cxx::rcm2rgrid<float>(lonArr2D.get(), latArr2D.get(), lonGridNum, latGridNum, lon_rged.data(), lat_rged.data(), lon_rged.size(), lat_rged.size(), u_unstged[time_i], u_regularGrid[time_i], 9.96921e+36, ier);

#               pragma omp atomic
                ++completed_count;

                if (completed_count % itsPerCheck == 0) {
#                   pragma omp critical
                    progressCallback(static_cast<double>(completed_count)/timeLength*100, target);
                }
            }
            assert(ier == 0);
            if (shouldCancel->load()) { return; }

            completed_count = 0;
#           pragma omp parallel for num_threads(threadNum)
            for (int time_i = 0; time_i < timeLength; ++time_i) {
                if (shouldCancel->load()) { continue; }

                NCL_cxx::rcm2rgrid<float>(lonArr2D.get(), latArr2D.get(), lonGridNum, latGridNum, lon_rged.data(), lat_rged.data(), lon_rged.size(), lat_rged.size(), v_unstged[time_i], v_regularGrid[time_i], 9.96921e+36, ier);

#               pragma omp atomic
                ++completed_count;

                if (completed_count % itsPerCheck == 0) {
#                   pragma omp critical
                    progressCallback(static_cast<double>(completed_count)/timeLength*100, target);
                }
            }
            assert(ier == 0);
            if (shouldCancel->load()) { return; }

            std::cout << "finish regrid wrfout uv" << std::endl;

            stepPgCallback(2, target);    // start cal rv

            vorField.setDims(timeLength, lat_rged.size(), lon_rged.size());
            for (int time_i = 0; time_i < timeLength; ++time_i) {
                uv2vr_cfd().calRV(u_regularGrid[time_i], v_regularGrid[time_i], lat_rged.data(), lon_rged.data(), lat_rged.size(), lon_rged.size(), 9.96921e+36, 2, vorField[time_i]);
            }
            if (shouldCancel->load()) { return; }
            std::cout << "finish cal wrfout rv" << std::endl;

            // treat regridded data as regular grid data
            wrfChangeToRegular = true;
            latArr = std::make_unique<float[]>(lat_rged.size());
            lonArr = std::make_unique<float[]>(lon_rged.size());
            std::copy(lat_rged.begin(), lat_rged.end(), latArr.get());
            std::copy(lon_rged.begin(), lon_rged.end(), lonArr.get());
//            latArr2D.get() = nullptr;
//            lonArr2D.get() = nullptr;

        } else {     // should not regrid
            vorField.setDims(timeLength, latGridNum, lonGridNum);
            stepPgCallback(2, target);    // start cal rv
            calcRelativeVorField(iiFile.get(), vorField);
        }
    } else {        // not wrfout file
        if (toGridRes > 0 && shouldRegrid(toGridRes)) {
            std::cout << "file should be regrid" << std::endl;
            auto ref_latData = getRgedLatArr(toGridRes);
            auto ref_lonData = getRgedLonArr(toGridRes);
            std::cout << "ref lat data:" << std::endl;
            for (float i : ref_latData) { std::cout << i << " "; }
            std::cout << "\nref lon data:" << std::endl;
            for (float i : ref_lonData) { std::cout << i << " "; }
            std::cout << std::endl;

            stepPgCallback(1, target);      // start regrid

            vorField.setDims(timeLength, ref_latData.size(), ref_lonData.size());

            if (!varNames.dataIsVor) {
                auto uField = ThreeDArray(timeLength, ref_latData.size(), ref_lonData.size());
                auto vField = ThreeDArray(timeLength, ref_latData.size(), ref_lonData.size());

                regridTheVarData(ref_latData, ref_lonData, varNames.uwndVarName, uField, progressCallback, target);
                if (shouldCancel->load()) { return; }
                regridTheVarData(ref_latData, ref_lonData, varNames.uwndVarName, vField, progressCallback, target);
                if (shouldCancel->load()) { return; }

                refreshRgedLatLonData(ref_latData, ref_lonData);

                stepPgCallback(2, target);         // start cal rv
                calculateRV(uField, vField, vorField);
                if (shouldCancel->load()) { return; }
            } else {
                regridTheVarData(ref_latData, ref_lonData, varNames.vorVarName, vorField, progressCallback, target);
                if (shouldCancel->load()) { return; }
                refreshRgedLatLonData(ref_latData, ref_lonData);
            }

        } else {          // should not regrid
            vorField.setDims(timeLength, latGridNum, lonGridNum);
            if (!varNames.dataIsVor) {
                auto uField = ThreeDArray(timeLength, latGridNum, lonGridNum);
                auto vField = ThreeDArray(timeLength, latGridNum, lonGridNum);
                auto uVar = iiFile->getVar(varNames.uwndVarName);

                if (zLevelIndex == -1) {
                    uVar.getVar(uField.get());
                    iiFile->getVar(varNames.vwndVarName).getVar(vField.get());
                } else {
                    uVar.getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { timeLength,1,latGridNum,lonGridNum }, uField.get());
                    iiFile->getVar(varNames.vwndVarName).getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { timeLength,1,latGridNum,lonGridNum }, vField.get());
                }

                float fillValue = 0.0;
                if (UtilFunc::checkIfHasVarAtt(uVar, "_FillValue")) {
                    uVar.getAtt("_FillValue").getValues(&fillValue);
                } else {
                    fillValue = 9.96921e+36;
                }

                stepPgCallback(2, target);         // start cal rv
                for (int time_i = 0; time_i < timeLength; ++time_i) {
                    uv2vr_cfd().calRV(uField[time_i], vField[time_i], latArr.get(), lonArr.get(), latGridNum, lonGridNum, fillValue, 2, vorField[time_i]);
                }

            } else {
                if (zLevelIndex == -1) {
                    iiFile->getVar(varNames.vorVarName).getVar(vorField.get());
                } else {
                    iiFile->getVar(varNames.vorVarName).getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { timeLength,1,latGridNum,lonGridNum }, vorField.get());
                }
            }
        }
    }
    Constants::RECURSION_MIN_ReVOR = std::abs(vorField.avgMinValue(threadNum));
//    Constants::RECURSION_MIN_ReVOR = 0.00005;
    //Constants::HAS_TP_MIN_ReVOR = isWrfoutFile ? 100e-5 : 8e-5;
    std::cout << "RECURSION_MIN_ReVOR: " << Constants::RECURSION_MIN_ReVOR << std::endl;

    int completed_count = 0;
    unsigned long itsPerCheck = timeLength / 20;

    stepPgCallback(3, target);         // start getVortexNum1Time
#   pragma omp parallel for num_threads(threadNum)
    for (int timeIndex = 0; timeIndex < timeLength; ++timeIndex) {
        // std::cout << vorVar.getName() << std::endl;
//        if (timeIndex % itsPerCheck == 0) {
//            //std::cout << static_cast<int>(timeIndex / static_cast<float>(timeLength) * 100) << "%" << std::endl;
//            std::cout << timeIndex << std::endl;
//            if (*isCanceled) {
//                std::cout << "msg from step1: Canceled!!!" << std::endl;
//                return;
//            }
//        }
        // vorVar.getVar({timeIndex,0,0}, {1, latGridNum, lonGridNum}, vorField.get());

        if (shouldCancel->load()) { continue; }

        getVortexNum1Time(vorField, timeIndex);
//        TCNum_prevTime = tpNum_timei;

#       pragma omp atomic
        ++completed_count;

        if (completed_count % itsPerCheck == 0) {
#       pragma omp critical
            progressCallback(static_cast<double>(completed_count)/timeLength*100, target);
        }
    }
//    dumpVortexes(allVorsCellsIndex);
//    progressCallback(100, target);
    std::cout << "Done step1" << std::endl;

}

/// 第二步：跟踪第一步生成的每个时次的气旋，生成真正的气旋对象。
void Processor::getRealTC() {
    std::cout << "start get real TC" << std::endl;
    UtilFunc::modifyMaxDist(iiFile.get(), isWrfoutFile ? "XTIME" : varNames.timeVarName);

    /// 当前时次和前一时次是否有气旋
    bool hasTCCurrentTime = false, hasTCPrevTime = false;
    /// 气旋编号，从1开始
    int TC_No = 1;
    /// 当前时次的index
    //int timeIndex = 0;
    ///
    //int hasVortex_notHandledYetIndex = 0;
    /// 最后有气旋的时次的index+1
    int hasTCLastTimeIndex = getLastNotEmptyVecIndex();
    /// 跟踪时仍未确定消亡日期的气旋；跟踪完成的气旋
    std::vector<Typhoon> tempTCs{};

    for (int timeIndex = 0; timeIndex < hasTCLastTimeIndex; ++timeIndex) {
        if (timeIndex % 1000 == 0)
            std::cout << timeIndex << std::endl;
        /// 当前时次是否有气旋？
//        hasTCCurrentTime = (std::find(hasTC_timeIndex.begin(), hasTC_timeIndex.end(), timeIndex) != hasTC_timeIndex.end()) ? true : false;
        hasTCCurrentTime = !allVortexes[timeIndex].empty();
        if (!hasTCCurrentTime) {
            if (hasTCPrevTime) {  // 当前时次无气旋，前一时次有气旋，确定气旋消亡日期
                for (auto &tempTC : tempTCs)
                    tempTC.endTimeIndex = timeIndex - 1;
                realTCs.insert(realTCs.end(), tempTCs.begin(), tempTCs.end());
                tempTCs.clear();
            }

        } else {             // 当前时次有气旋
            /// 当前时次的涡旋vector
            //auto currentTimeVortexes = allVortexes[hasVortex_notHandledYetIndex];
            auto currentTimeVortexes = allVortexes[timeIndex];
            /// 当前时次的涡旋个数
            int currentTimeTCNum = currentTimeVortexes.size();
            //++hasVortex_notHandledYetIndex;

            if (!hasTCPrevTime) {  // 当前时次有气旋，但前一时次无气旋，创建新的气旋对象

                for (int i = 0; i < currentTimeTCNum; ++i) {
                    tempTCs.push_back(Typhoon{
                        i+TC_No, {currentTimeVortexes[i].maxVorCellIndex}, {currentTimeVortexes[i].geoCenter}, timeIndex, timeIndex, {currentTimeVortexes[i].cellsIndex}
                    });
                }
                // 更新气旋编号，注意更新后的编号所代表的气旋仍未出现
                TC_No += currentTimeTCNum;
            } else {             // 当前时次和前一时次都有气旋
                int tempTCsSize = tempTCs.size();
                int currentTVortexesSize = currentTimeVortexes.size();

                /// 当前时次的涡旋index
                // std::vector<TC1Time> currentTVortexes_copy(currentTimeVortexes);
                std::set<int> currentT_VorsRMIndex;
                // std::iota(currentT_VorsIndex.begin(), currentT_VorsIndex.end(), 0);

                /// 正在跟踪的气旋index
                // std::vector<Typhoon> tempTCS_copy(tempTCs);
                std::set<int> tempTCsRMIndex;

                /// 正在跟踪的气旋index，用于判断每个台风是否被处理
                // std::vector<Typhoon> tempTCS_ForD(tempTCs);
                std::set<int> tempTCsIndexForD;
                for (int i = 0; i < tempTCsSize; ++i) {
                    // tempTCsIndex.insert(tempTCsIndex.end(), i);
                    tempTCsIndexForD.insert(tempTCsIndexForD.end(), i);
                }


                // 涡度最大点index实现
                //std::vector<std::pair<int, int>> temp_TCs_max_pt, currentT_TCs_maxVorCellIndex;
                //std::transform(tempTCs.begin(), tempTCs.end(), std::back_inserter(temp_TCs_max_pt), [](const Typhoon &temp_TC) {return temp_TC.maxVorCells.back();});
                //std::transform(currentTimeVortexes.begin(), currentTimeVortexes.end(), std::back_inserter(currentT_TCs_maxVorCellIndex), [](const TC1Time &currentTimeVortex) {return currentTimeVortex.maxVorCellIndex;});

                // 几何中心实现
                std::vector<std::pair<float, float>> tempTCsGeoCenters, currentT_TCs_GeoCenters;
                std::transform(tempTCs.begin(), tempTCs.end(), std::back_inserter(tempTCsGeoCenters), [](const Typhoon& temp_TC) {return temp_TC.geoCenters.back(); });
                std::transform(currentTimeVortexes.begin(), currentTimeVortexes.end(), std::back_inserter(currentT_TCs_GeoCenters), [](const TC1Time& currentTimeVortex) {return currentTimeVortex.geoCenter; });

                /// 距离矩阵（多维数组实现）
                // float distMatrix[tempTCsSize][currentTVortexesSize];
                // for (int i = 0; i < tempTCsSize; ++i) {
                //     for (int j = 0; j < currentTVortexesSize; ++j) {
                //         distMatrix[i][j] = UtilFunc::cellDist(latArr, lonArr, temp_TCs_max_pt[i], currentT_TCs_maxVorCellIndex[j]);
                //     }
                // }
                /// 距离矩阵（pointer实现）
                // float **distMatrix;
                // distMatrix = new float *[tempTCsSize];
                // for (int i = 0; i < tempTCsSize; ++i)
                //     distMatrix[i] = new float[currentTVortexesSize];
                // float **distMatrix = new float[tempTCsSize][currentTVortexesSize];

                auto distMatrix = TwoDArray(tempTCsSize, currentTVortexesSize);

                // 涡度最大点index实现
                //for (int i = 0; i < tempTCsSize; ++i) {
                //    for (int j = 0; j < currentTVortexesSize; ++j) {
                //        distMatrix(i,j) = UtilFunc::cellDist(latArr.get(), lonArr.get(), temp_TCs_max_pt[i], currentT_TCs_maxVorCellIndex[j]);
                //    }
                //}
                // 几何中心实现
                for (int i = 0; i < tempTCsSize; ++i) {
                    for (int j = 0; j < currentTVortexesSize; ++j) {
                        auto [lat1, lon1] = tempTCsGeoCenters[i];
                        auto [lat2, lon2] = currentT_TCs_GeoCenters[j];
                        distMatrix(i,j) = UtilFunc::cellDist(lat1, lon1, lat2, lon2);
                    }
                }

                while (!tempTCsIndexForD.empty()) {
                    if (currentT_VorsRMIndex.size() == currentTVortexesSize) {
                        // 如果进入了这个条件，就说明当前时次的气旋比昨天的少，当前时次的气旋都已经对应成功
                        // 现在处理的是上一个时次的没有对应的气旋

                        /// 需要被删除的气旋在index中的iterator（删除第一个）
                        auto willBeRMTCIter = tempTCsIndexForD.begin();
                        realTCs.push_back(tempTCs[*willBeRMTCIter]);
                        // 这里必须加星号
                        tempTCsRMIndex.insert(*willBeRMTCIter);
                        // 注意这里是删除第一个元素，至于加不加星号都是删除第一个元素，加星号意味着根据值删除
                        tempTCsIndexForD.erase(willBeRMTCIter);
                        continue;
                    }
                    // 寻找两个靠的最近的昨日台风与今日台风
                    // auto minDist = UtilFunc::min_element_2d(distMatrix, tempTCsSize, currentTVortexesSize);
                    auto minDist = distMatrix.min();
                    auto minDistIndex = minDist.first;
                    auto minD_tempTC = tempTCs[minDistIndex.first];
                    auto minD_currentTTC = currentTimeVortexes[minDistIndex.second];

                    if (minDist.second > Constants::LINK_TP_MAX_DIST) {  // 该台风昨天就消亡了
                        realTCs.push_back(minD_tempTC);

                        // 从临时数组中移去已消亡的台风
                        tempTCsRMIndex.insert(minDistIndex.first);
                        tempTCsIndexForD.erase(minDistIndex.first);
                        // tempTCS_copy.erase(std::remove(tempTCS_copy.begin(), tempTCS_copy.end(), minD_tempTC));
                        // tempTCS_ForD.erase(std::remove(tempTCS_ForD.begin(), tempTCS_ForD.end(), minD_tempTC));

                        // 处理完后将距离设为一个较大的值
                        // 不可设置distMatrix[:][minDistIndex.second] = 1e5！！！！
                        for (int i = 0; i < currentTVortexesSize; ++i) {
                            distMatrix(minDistIndex.first, i) = 1e5;
                        }
                    } else {                              // 对应成功，移除今天对应的台风
                        // auto theIndex = std::find(tempTCS_copy.begin(), tempTCS_copy.end(), minD_tempTC);
                        // 更新台风移动到的位置
                        // theIndex->maxVorCells.push_back(minD_currentTTC.maxVorCellIndex);
                        tempTCs[minDistIndex.first].maxVorCells.push_back(minD_currentTTC.maxVorCellIndex);
                        // 更新最大风速
                        //
                        // 更新台风长轴中心移动到的位置
                        //
                        // 更新台风四点中心移动到的位置
                        //
                        // 更新台风几何中心移动到的位置
                        tempTCs[minDistIndex.first].geoCenters.push_back(minD_currentTTC.geoCenter);
                        // 更新台风消亡时间
                        tempTCs[minDistIndex.first].endTimeIndex = timeIndex;
                        // 更新台风涡旋包含的点的index
                        tempTCs[minDistIndex.first].vorsCellsIndex.push_back(minD_currentTTC.cellsIndex);

                        tempTCsIndexForD.erase(minDistIndex.first);
                        // 移除对应成功的当前时次的涡旋
                        // currentTVortexes_copy.erase(std::remove(currentTVortexes_copy.begin(), currentTVortexes_copy.end(), minD_currentTTC));
                        currentT_VorsRMIndex.insert(minDistIndex.second);

                        for (int i = 0; i < currentTVortexesSize; ++i)
                            distMatrix(minDistIndex.first, i) = 1e5;
                        for (int i = 0; i < tempTCsSize; ++i)
                            distMatrix(i, minDistIndex.second) = 1e5;
                    }
                }
                // 根据index删除相应的消亡的气旋
                for (auto rIter = tempTCsRMIndex.crbegin(); rIter != tempTCsRMIndex.crend(); ++rIter)
                    tempTCs.erase(tempTCs.cbegin() + *rIter);

                // 前一时次的所有的气旋对应完，today_tps_copy剩下新生成的、未被跟踪的气旋
                if (currentT_VorsRMIndex.size() < currentTVortexesSize) {
                    // 根据index删除相应的处理完的当前时次的涡旋
                    for (auto rIter = currentT_VorsRMIndex.crbegin(); rIter != currentT_VorsRMIndex.crend(); ++rIter) {
                        currentTimeVortexes.erase(currentTimeVortexes.cbegin() + *rIter);
                    }
                    int currentTTCsNum = currentTimeVortexes.size();
                    for (int i = 0; i < currentTTCsNum; ++i) {
                        tempTCs.push_back(Typhoon{
                            TC_No+i, {currentTimeVortexes[i].maxVorCellIndex}, {currentTimeVortexes[i].geoCenter}, timeIndex, timeIndex, {currentTimeVortexes[i].cellsIndex}
                        });
                    }
                    // 更新台风编号，注意更新后的编号所代表的台风仍未出现
                    TC_No += currentTTCsNum;
                }

            }

            // TODO: 如果当前时次是最后一个时次，应处理tempTC

        }
        hasTCPrevTime = hasTCCurrentTime;
    }
    for (auto& tempTC : tempTCs)
        tempTC.endTimeIndex = hasTCLastTimeIndex - 1;
    realTCs.insert(realTCs.end(), tempTCs.begin(), tempTCs.end());
    tempTCs.clear();

    std::cout << "msg from getRealTC, realTC number" << realTCs.size() << std::endl;
}

/// 第三步：移除噪声
void Processor::removeNoise() {
    int realTCsNum = realTCs.size();
    /// 需移除的气旋的index的set（会自动按升序排列）
    std::set<int> tcRMIndex;

    auto addRMIndex = [&tcRMIndex](int i) {
        tcRMIndex.insert(i);
    };

    // 获取陆地信息
    //std::vector<std::vector<std::pair<float, float>>> landPolygons;
    getLandPolygons();

    for (int i = 0; i < realTCsNum; ++i) {
        auto realTC = realTCs[i];
        //float tcStartLat = latArr[realTC.maxVorCells.front().first], tcStartLon = lonArr[realTC.maxVorCells.front().second];
        //float tcEndLat = latArr[realTC.maxVorCells.back().first], tcEndLon = lonArr[realTC.maxVorCells.back().second];
        auto [tcStartLat, tcStartLon] = realTC.geoCenters.front();
        auto [tcEndLat, tcEndLon] = realTC.geoCenters.back();

        // 去除不动的点（即包括单个点）
        if ((tcStartLon == tcEndLon) && (tcStartLat == tcEndLat)) {
            addRMIndex(i);
            // 去除小于3个点的轨迹
        } else if (realTC.maxVorCells.size() < 9) {
            addRMIndex(i);
            // 去除一直在东边的轨迹
        } else if ((tcStartLon >= 170) && (tcEndLon >= 170)) {
            addRMIndex(i);
            // 排除非热带生成的气旋
        } else if ((tcStartLat >= 33) || (tcStartLat < 0)) {
            addRMIndex(i);
            // 去除温带低压
        } else if (UtilFunc::alwaysMoveEast(realTC.maxVorCells)) {
            addRMIndex(i);
            // 第一个点在陆地上
        } else if (pnpolys(tcStartLat, tcStartLon)) {
            addRMIndex(i);
            // 排除纬度太低的气旋
            //} else if ((tcEndLat < 5) && UtilFunc::cellsLatAvg(latArr.get(), realTC.maxVorCells) < 5) {
        } else if ((tcEndLat < 5) && UtilFunc::cellsLatAvg(realTC.geoCenters) < 5) {
            addRMIndex(i);
            // 排除印度洋的台风
            //} else if (UtilFunc::cellsLonAvg(lonArr.get(), realTC.maxVorCells) < 103) {
        } else if (UtilFunc::cellsLonAvg(realTC.geoCenters) < 103) {
            addRMIndex(i);
        }
    }
    // 按index降序逐个删除需删除的气旋
    for (auto rIter = tcRMIndex.crbegin(); rIter != tcRMIndex.crend(); ++rIter)
        realTCs.erase(realTCs.cbegin() + *rIter);

    std::cout << "remove noise completed" << std::endl;
    std::cout << "msg from removeNoise, realTC number: " << realTCs.size() << std::endl;

//    dumpStep3_proto3("/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/JRA-55_general.protobuf");
//    dumpStep3_proto3("/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/modelResult/CESM_result_1pt5.protobuf");
}

/// 此方法识别某个时次是否有台风以及台风的个数
/// @param[in] vorField 涡度场（时间、纬度、经度）
/// @param[in] timeIndex 时间index
/// @return 当前时次的涡旋数量
int Processor::getVortexNum1Time(ThreeDArray &vorField, int timeIndex) {
    /// 当前时次的涡旋的个数
    int tpNum = 0;
    std::vector<TC1Time> vortexesThisTime{};
    /// 当前时次所有vortex的cells的index
    std::vector<std::unordered_set<std::pair<int, int>, pair_hash>> vortexesCellsIndex;

    for (int i = 0; i < Constants::TODAY_MAX_TP_NUM; ++i) {
        // auto maxVorCell = UtilFunc::max_element_2d(vorField);
        auto maxVorCell = vorField.max(timeIndex);
        /// 当前vortex的cells的index
        std::unordered_set<std::pair<int, int>, pair_hash> allCellsIndex;
        getVortexCellsIndex(vorField, timeIndex, maxVorCell.first, allCellsIndex);

        // auto a = allCellsIndex.begin();
        // while (a != allCellsIndex.end()) {
        //     std::cout << "(" << a->first << "," << a->second << ") ";
        //     ++a;
        // }
        // std::cout << std::endl;

        if (allCellsIndex.empty())
            break;
//        if ((allCellsIndex.size() <= Constants::TP_MIN_PTS) && (TCNum_prevTime == 0) )
//            break;

//        if (!isWrfoutFile) {
//            float e = get_e(allCellsIndex);
//            if ((e > Constants::TP_MIN_E) && (TCNum_prevTime == 0))
//                break;
//        }

        ++tpNum;
        auto vortexCenterLatLon = isWrfoutFile && !wrfChangeToRegular ? UtilFunc::getVortexCenterLatLon(allCellsIndex, latArr2D, lonArr2D) : UtilFunc::getVortexCenterLatLon(allCellsIndex, latArr.get(), lonArr.get());
        vortexesCellsIndex.push_back(allCellsIndex);
        vortexesThisTime.push_back(TC1Time{maxVorCell.first, vortexCenterLatLon, set2Vector(allCellsIndex)});
        removeVortex(vorField, timeIndex, allCellsIndex);
    }
//    allVortexes.push_back(vortexesThisTime);
    allVorsCellsIndex[timeIndex] = vortexesCellsIndex;
    allVortexes[timeIndex] = vortexesThisTime;
    return tpNum;

}

/// 此函数接受一个点，递归返回所有在台风内的点（阈值采用相对涡度）
/// @param[in] vorField 涡度场（2d array）
/// @param[in] maxValIndex 涡度最大值的格点对应的纬度、经度index
void Processor::getVortexCellsIndex(ThreeDArray &vorField, int timeIndex, std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCellsIndex) {
    if (vorField(timeIndex, maxValIndex.first, maxValIndex.second) >= Constants::RECURSION_MIN_ReVOR) {
        if (!allCellsIndex.count(maxValIndex)) {
            allCellsIndex.insert(maxValIndex);
            auto surroundingCellsIndex = getSurroundingCellsIndex(maxValIndex);
            for (auto &cellIndex: surroundingCellsIndex) {
                getVortexCellsIndex(vorField, timeIndex, cellIndex, allCellsIndex);
            }
        }
    }
}

/// 此函数接受一个点，返回周围的点（最多8个），如果在边缘则排除边缘外的点。
/// @param[in] cellIndex 点的index
/// @return 周围的点的index组成的vector
std::unordered_set<std::pair<int, int>, pair_hash> Processor::getSurroundingCellsIndex(std::pair<int, int> cellIndex) {
    std::unordered_set<std::pair<int, int>, pair_hash> surroundingCells{
        // 左列三个点
        {cellIndex.first+1, cellIndex.second-1}, {cellIndex.first, cellIndex.second-1}, {cellIndex.first-1, cellIndex.second-1},
        // 上下两个点
        {cellIndex.first+1, cellIndex.second}, {cellIndex.first-1, cellIndex.second},
        // 右列三个点
        {cellIndex.first+1, cellIndex.second+1}, {cellIndex.first, cellIndex.second+1}, {cellIndex.first-1, cellIndex.second+1}
    };
    // 点在左边缘
    if (cellIndex.second == 0) {
        std::vector<std::pair<int, int>> excludeCells{{cellIndex.first+1, cellIndex.second-1}, {cellIndex.first, cellIndex.second-1}, {cellIndex.first-1, cellIndex.second-1}};
        for (auto &i : excludeCells) {
            surroundingCells.erase(i);
        }
    } else if (cellIndex.second == lonGridNum - 1) {      // 点在右边缘
        std::vector<std::pair<int, int>> excludeCells{{cellIndex.first+1, cellIndex.second+1}, {cellIndex.first, cellIndex.second+1}, {cellIndex.first-1, cellIndex.second+1}};
        for (auto &i : excludeCells) {
            surroundingCells.erase(i);
        }
    }
    // 点在上边缘
    if (cellIndex.first == latGridNum - 1) {
        std::vector<std::pair<int, int>> excludeCells{{cellIndex.first+1, cellIndex.second-1}, {cellIndex.first+1, cellIndex.second}, {cellIndex.first+1, cellIndex.second+1}};
        for (auto &i : excludeCells) {
            surroundingCells.erase(i);
        }
    } else if (cellIndex.first == 0) {                         // 点在下边缘
        std::vector<std::pair<int, int>> excludeCells{{cellIndex.first-1, cellIndex.second-1}, {cellIndex.first-1, cellIndex.second}, {cellIndex.first-1, cellIndex.second+1}};
        for (auto &i : excludeCells) {
            surroundingCells.erase(i);
        }
    }
    return surroundingCells;
}

/// 计算偏心率（未实现wrfout文件的偏心率计算）
/// @param vortexCellsIndex 表示涡旋的cells的index
/// @return 偏心率
float Processor::get_e(std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex) {
    std::vector<std::pair<int, int>> vortexCellsIndex_v(vortexCellsIndex.begin(), vortexCellsIndex.end());
    auto distMax = UtilFunc::getMaxDistance(vortexCellsIndex_v);

    //float gridRatio = latArr[1] - latArr[0];
    float gridRatio = 1.0;
    /// 半长轴长度（单位：度）
    float A = distMax.second * gridRatio / 2.0;
    if (A == 0) {
        // std::cout << "A == 0!" << std::endl;
        return 1.0;
    }
    /// 长轴斜率
    float majorAxisK = UtilFunc::getSlope(vortexCellsIndex_v[distMax.first.first], vortexCellsIndex_v[distMax.first.second]);
    /// 短轴斜率
    float minorAxisK = (majorAxisK == 0) ? std::numeric_limits<float>::infinity() : (-1 / majorAxisK);
    /// 椭圆中点纬度、经度
    auto centerLatLon = UtilFunc::getCellsCenterLatLon(vortexCellsIndex_v[distMax.first.first], vortexCellsIndex_v[distMax.first.second], latArr.get(), lonArr.get());

    /// 半短轴估计值
    auto minorAxis = getMinorAxisLen(vortexCellsIndex_v, centerLatLon, minorAxisK, gridRatio, A);
    float approB = minorAxis.first / 2.0;
    if (approB > A)
        std::swap(approB, A);

    float E = std::sqrt((A*A-approB*approB)/(A*A-approB*approB*minorAxis.second*minorAxis.second));
    return E;

}

/// 此方法计算短轴长度
std::pair<float, float> Processor::getMinorAxisLen(const std::vector<std::pair<int, int>> &cellsIndex, const std::pair<float, float> &centerLatLon, float minorAxisK, const float gridRatio, const float A) {
    /// 涡旋cell的纬度经度array
    //std::pair<float, float> cellsLatLon[cellsIndex.size()];
    auto cellsLatLon = std::make_unique<std::pair<float,float>[]>(cellsIndex.size());
    std::transform(cellsIndex.begin(), cellsIndex.end(), cellsLatLon.get(), [this](const std::pair<int, int> &cellIndex) -> std::pair<float, float> {
        return {latArr[cellIndex.first], lonArr[cellIndex.second]};
    });
    std::vector<std::pair<int, int>> validCellsIndex;
    /// 短轴轴线对应的弧度
    float minorAxisRad = std::atan(minorAxisK);
    for (int i = 0; i < cellsIndex.size(); ++i) {    // 挑选处于短轴线左右30度所包括的所有点
        /// 椭圆中心点与某点的斜率
        auto cellRad = std::atan(UtilFunc::getSlope(centerLatLon, cellsLatLon[i]));
        if (std::abs(minorAxisRad - cellRad) <= M_PI / 4)
            validCellsIndex.push_back(cellsIndex[i]);
    }
    // 如果没有一个点或只有一个点符合条件，说明太窄，排除
    if (validCellsIndex.size() <= 1)
        return {0.0, 1.0};

    /// 取这些点中两个距离最远的点
    auto approMinorAxis = UtilFunc::getMaxDistance(validCellsIndex);
    /// 两个距离最远的点连线的弧度
    float appro_min_rad = std::atan(UtilFunc::getSlope(validCellsIndex[approMinorAxis.first.first], validCellsIndex[approMinorAxis.first.second]));
    /// 估计线与真实线夹角余弦
    // float cosAlpha = std::abs(std::cos(appro_min_rad - minorAxisRad));
    // if (cosAlpha == 0)               // 短轴和长轴重合！
    //     return 0.0;                 // 可能使计算出的斜率严重出错（即与长轴有相同趋势）

    float approMinorAxisDist = approMinorAxis.second * gridRatio;

    return {approMinorAxisDist, std::sin(std::abs(appro_min_rad - minorAxisRad))};

}

/// 此函数消除相对涡度大值中心，为识别多个台风服务，替换为1e-6
/// @param[in,out] vorField 涡度场（2d array）
/// @param[out] timeIndex 时间index
/// @param[out] vortexCellsIndex 涡旋包含的点的index
inline void Processor::removeVortex(ThreeDArray &vorField, int timeIndex, std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex) {
    for (auto &i : vortexCellsIndex) {
        vorField(timeIndex, i.first, i.second) = 1e-6;
    }
}

std::vector<std::pair<int, int>> Processor::set2Vector(const std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex) {
    std::vector<std::pair<int, int>> ret;
    ret.insert(ret.end(), vortexCellsIndex.begin(), vortexCellsIndex.end());
    return ret;
}

bool Processor::shouldRegrid(float spatialRes) {
    assert(latArr[1] > latArr[0]);
    assert(lonArr[1] > lonArr[0]);
    if (std::abs(latArr[1] - latArr[0] - spatialRes) > 1e-4 ) { return true; }
    if (std::abs(lonArr[1] - lonArr[0] - spatialRes) > 1e-4 ) { return true; }
    return false;
}

std::vector<float> Processor::getRgedLatArr(float spatialRes) {
    int minLatGridNum = static_cast<int>(latArr[0] / spatialRes);
    int maxLatGridNum = static_cast<int>(latArr[latGridNum-1] / spatialRes);
    int ref_latGridNum = maxLatGridNum - minLatGridNum + 1;
    std::vector<float> ref_latData(ref_latGridNum, 0);
//    std::iota(ref_latData.data(), ref_latData.data()+ref_latGridNum, minLatGridNum * spatialRes);
    float startLat = minLatGridNum * spatialRes - spatialRes;
    std::generate(ref_latData.data(), ref_latData.data()+ref_latGridNum, [&startLat, spatialRes]{ return startLat+=spatialRes; });
    return ref_latData;
}
std::vector<float> Processor::getRgedLonArr(float spatialRes) {
    int minLonGridNum = static_cast<int>(lonArr[0] / spatialRes);
    int maxLonGridNum = static_cast<int>(lonArr[lonGridNum-1] / spatialRes);
    int ref_lonGridNum = maxLonGridNum - minLonGridNum + 1;
    std::vector<float> ref_lonData(ref_lonGridNum, 0);
//    std::iota(ref_lonData.data(), ref_lonData.data()+ref_lonGridNum, minLonGridNum * spatialRes);
    float startLon = minLonGridNum * spatialRes - spatialRes;
    std::generate(ref_lonData.data(), ref_lonData.data()+ref_lonGridNum, [&startLon, spatialRes]{ return startLon+=spatialRes; });
    return ref_lonData;
}

void Processor::regridTheVarData(const std::vector<float> &ref_latData, const std::vector<float> &ref_lonData, const std::string &theVarName, ThreeDArray &theVarField, void(*progressCallback)(double progressValue, void*), void* target) {
    int ref_latGridNum = ref_latData.size(), ref_lonGridNum = ref_lonData.size();
    auto theVar = iiFile->getVar(theVarName);

    auto tempTheVarField = ThreeDArray(timeLength, latGridNum, lonGridNum);

    if (zLevelIndex == -1) {
        theVar.getVar(tempTheVarField.get());
    } else {
        theVar.getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { timeLength,1,latGridNum,lonGridNum }, tempTheVarField.get());
    }

    std::cout << "start regridding..." << std::endl;
//    auto interp = Linint2();

    int completed_count = 0;
    unsigned long itsPerCheck = timeLength / 50;

#   pragma omp parallel for num_threads(threadNum)
    for (int timeIndex = 0; timeIndex < timeLength; ++timeIndex) {
        if (shouldCancel->load()) { continue; }

        NCL_cxx::linint2(lonArr.get(), lonGridNum, latArr.get(), latGridNum, ref_lonData.data(), ref_lonGridNum, ref_latData.data(), ref_latGridNum, tempTheVarField.get()+timeIndex*lonGridNum*latGridNum, theVarField.get()+timeIndex*ref_lonGridNum*ref_latGridNum, false, -9999);

#       pragma omp atomic
        ++completed_count;

        if (completed_count % itsPerCheck == 0) {
#           pragma omp critical
            progressCallback(static_cast<double>(completed_count)/timeLength*100, target);
        }
    }

//    NCL_cxx::linint2(threadNum, timeLength, lonArr.get(), lonGridNum, latArr.get(), latGridNum, ref_lonData.data(), ref_lonGridNum, ref_latData.data(), ref_latGridNum, tempVorField.get(), vorField.get(), false, -9999);

}

void Processor::refreshRgedLatLonData(const std::vector<float> &newLatData, const std::vector<float> &newLonData) {
    // 将旧的lat和lon数据替换为regrid后的lat和lon
    int ref_latGridNum = newLatData.size(), ref_lonGridNum = newLonData.size();
    latGridNum = ref_latGridNum; lonGridNum = ref_lonGridNum;
    latArr = std::make_unique<float[]>(latGridNum);   // reassign, 不会导致内存泄漏
    lonArr = std::make_unique<float[]>(lonGridNum);   // reassign, 不会导致内存泄漏
    std::copy(newLatData.data(), newLatData.data()+latGridNum, latArr.get());
    std::copy(newLonData.data(), newLonData.data()+lonGridNum, lonArr.get());
}

void Processor::calculateRV(ThreeDArray &uField, ThreeDArray &vField, ThreeDArray &vorField) {
    float fillValue = 0.0;
    auto uVar = iiFile->getVar(varNames.uwndVarName);
    if (UtilFunc::checkIfHasVarAtt(uVar, "_FillValue")) {
        uVar.getAtt("_FillValue").getValues(&fillValue);
    } else {
        fillValue = 9.96921e+36;
    }

    for (int time_i = 0; time_i < timeLength; ++time_i) {
        uv2vr_cfd().calRV(uField[time_i], vField[time_i], latArr.get(), lonArr.get(), latGridNum, lonGridNum, fillValue, 2, vorField[time_i]);
    }
}

void Processor::unstaggerU(netCDF::NcFile *inFile, ThreeDArray &u_unstged, ThreeDArray &v_unstged) {
    size_t nt = inFile->getDim("Time").getSize();
    size_t ny = inFile->getDim("south_north").getSize();
    size_t nx = inFile->getDim("west_east").getSize();

    auto u_staggered = ThreeDArray(nt, ny, nx + 1);
    auto v_staggered = ThreeDArray(nt, ny + 1, nx);
    if (zLevelIndex == -1) {
        inFile->getVar("U").getVar({ 0,0,0 }, { nt,ny,nx + 1 }, u_staggered.get());
        inFile->getVar("V").getVar({ 0,0,0 }, { nt,ny + 1,nx }, v_staggered.get());
    } else {
        inFile->getVar("U").getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { nt,1,ny,nx + 1 }, u_staggered.get());
        inFile->getVar("V").getVar({ 0,static_cast<size_t>(zLevelIndex),0,0 }, { nt,1,ny + 1,nx }, v_staggered.get());
    }

    for (int t_i = 0; t_i < nt; ++t_i) {
        for (int y_i = 0; y_i < ny; ++y_i) {
            for (int x_i = 0; x_i < nx; ++x_i) {
                u_unstged(t_i,y_i,x_i) = 0.5*(u_staggered(t_i,y_i,x_i)+u_staggered(t_i,y_i,x_i+1));
                v_unstged(t_i,y_i,x_i) = 0.5*(v_staggered(t_i,y_i,x_i)+v_staggered(t_i,y_i+1,x_i));
            }
        }
    }
}

int Processor::getLastNotEmptyVecIndex() {
    int i = allVortexes.size() - 1;
    while (allVortexes[i].empty()) { --i; }
    return i;
}

/// @brief 此方法检查一个文件夹名字对应的路径是否为路径
/// @param[in] folderName 文件夹名称
void Processor::checkDirAndCreate(const std::string& folderName) {
    //std::filesystem::path dumpDir("E:\\University\\TC_Tracker\\data\\stepFile\\");
    if (!dumpDir.empty()) {
        return;
    }
    std::cout << "未设定中间文件的储存位置，将中间文件储存在exe文件的位置下" << std::endl;
    dumpDir = boost::dll::program_location().parent_path().string();
    std::filesystem::path dumpDir(dumpDir);
    std::filesystem::path folderPath = dumpDir / folderName;
    this->dumpDir = folderPath.string();

    // 文件或文件夹不存在，创建文件夹
    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directories(folderPath);
        return;
    }
    // 存在文件夹，直接返回
    if (std::filesystem::is_directory(folderPath)) { return; }
    // 存在文件，重命名原有文件，并创建新文件夹
    std::filesystem::rename(folderPath, dumpDir / (folderName+".original"));
    std::filesystem::create_directories(folderPath);

}

/// 将第一步生成的Vortex数据用boost序列化为文件
/// @param[in] ncFilePath 文件绝对路径
void Processor::dumpStep1(const std::string ncFilePath) {
    //checkDirAndCreate("step1");

    std::filesystem::path stepDumpDir(dumpDir);
    std::ofstream ofs(stepDumpDir / ( std::filesystem::path(ncFilePath).stem().string() + "_step1.dat" ), std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    // write class instance to archive
    //        oa << hasTC_timeIndex << vortexes;
    Vortexes vortexes(allVortexes, tcInfo);
    oa << vortexes;
}

/// 将二步生成的TC数据用boost序列化为文件
/// @param[in] ncFilePath 文件绝对路径
void Processor::dumpStep2(const std::string ncFilePath) {
    //checkDirAndCreate("step2");
    std::filesystem::path stepDumpDir(dumpDir);
    std::ofstream ofs(stepDumpDir / (std::filesystem::path(ncFilePath).stem().string() + "_step2.dat"), std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    TCs tcs(realTCs, tcInfo);
    oa << tcs;

    //getLandPolygons();
    //std::cout << "test" << std::endl;
    //std::cout << pnpolys(24.764169, 112.762106) << std::endl;  // true
    //std::cout << pnpolys(23.628600, 120.898879) << std::endl;  // true
    //std::cout << pnpolys(19.745883, 115.252048) << std::endl;  // false
    //std::cout << pnpolys(7.034187, 109.994884) << std::endl;  // false
    //std::cout << pnpolys(16.830964, 121.199621) << std::endl;  // true
    //std::cout << pnpolys(7.871843, 124.967880) << std::endl;  // true
    //std::cout << pnpolys(20.260121, 107.827437) << std::endl;  // false
}

/// 将三步生成的TC数据用boost序列化为文件
/// @param[in] ncFilePath 文件绝对路径
void Processor::dumpStep3(const std::string ncFilePath) {
    //checkDirAndCreate("step3");
    std::filesystem::path stepDumpDir(dumpDir);
    std::ofstream ofs(stepDumpDir / (std::filesystem::path(ncFilePath).stem().string() + "_step3.dat"), std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    TCs tcs(realTCs, tcInfo);
    oa << tcs;
}

/// 读取dumpStep1生成的boost序列化文件并将数据读到vortexes属性
/// @param[in] filePath 文件绝对路径
void Processor::getStep1DataFromFile(const std::string& filePath) {
    std::ifstream ifs(filePath, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);

    allVortexes.clear();
    //        ia >> hasTC_timeIndex >> vortexes;
    Vortexes vortexes;
    ia >> vortexes;
    allVortexes = vortexes.getVortexes();
    tcInfo = vortexes.getTcInfo();
}

/// 读取dumpStep2生成的boost序列化文件并将数据读到tcs
/// @param[in] filePath 文件绝对路径
void Processor::getStep2DataFromFile(const std::string& filePath) {
    std::ifstream ifs(filePath, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);

    realTCs.clear();
    TCs tcs;
    ia >> tcs;
    realTCs = tcs.getTcs();
    tcInfo = tcs.getTcInfo();

}

/// 将第1步生成的vortexes输出为nc文件
/// @param allVorsCellsIndex 包含所有时次的vortex数据的vector
void Processor::dumpVortexes(const std::vector<std::vector<std::unordered_set<std::pair<int, int>, pair_hash>>> &allVorsCellsIndex) {
    VortexesDumper dumper(iFilePath, "/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/ERA5_onlyVortexes.nc", tcInfo);
    dumper.setLatLonData(latArr.get(), latGridNum, lonArr.get(), lonGridNum);
    dumper.dumpVortexes2NC(allVorsCellsIndex);
}

void Processor::copyRealTCs(std::vector<Typhoon>& tcs) {
    tcs = realTCs;
    std::cout << "msg from copy, realTCs number: " << tcs.size() << std::endl;
    std::cout << "copy tcs completed." << std::endl;
}

void Processor::copyTCs(TCs &tcs) {
    tcs = TCs(realTCs, tcInfo);
}
void Processor::copyLatLonData(std::vector<float> &lat_data, std::vector<float> &lon_data) {
    lat_data.clear();
    lon_data.clear();
    std::copy(latArr.get(), latArr.get()+latGridNum, std::back_inserter(lat_data));
    std::copy(lonArr.get(), lonArr.get()+lonGridNum, std::back_inserter(lon_data));
}

/// 读取GeoJSON大陆mask文件并将数据读到landPolygons
void Processor::getLandPolygons() {
    /// Resource文件夹位置(macOS)或可执行文件位置(CLI)
    auto resBaseDirStr = resourceBaseDir.empty() ? boost::dll::program_location().parent_path().string() : resourceBaseDir;

    std::filesystem::path resBaseDir(resBaseDirStr);
    auto mapPath = resBaseDir / "data" / "myMap.json";
    std::ifstream ifs(mapPath);
    if (!ifs) {
        throw std::runtime_error("Cannot open map file: " + mapPath.string());
    }

    nlohmann::json mapJson;
    ifs >> mapJson;

    landPolygons.clear();

    auto addPolygon = [this](const nlohmann::json& linearRing) {
        std::vector<std::pair<float, float>> polygon;
        polygon.reserve(linearRing.size());
        for (const auto& point : linearRing) {
            if (!point.is_array() || point.size() < 2) {
                throw std::runtime_error("Invalid coordinate in myMap.json");
            }
            polygon.emplace_back(point.at(1).get<float>(), point.at(0).get<float>());
        }
        if (!polygon.empty()) {
            landPolygons.push_back(std::move(polygon));
        }
    };

    for (const auto& feature : mapJson.at("features")) {
        const auto& geometry = feature.at("geometry");
        const auto geometryType = geometry.at("type").get<std::string>();
        const auto& coordinates = geometry.at("coordinates");

        if (geometryType == "Polygon") {
            addPolygon(coordinates.at(0));
        } else if (geometryType == "MultiPolygon") {
            for (const auto& polygon : coordinates) {
                addPolygon(polygon.at(0));
            }
        }
    }

    if (landPolygons.empty()) {
        throw std::runtime_error("No land polygons loaded from: " + mapPath.string());
    }
}


/// @brief 检查点是否在多边形里
/// @param[in] polygon 多边形
/// @param[in] testLat 点的纬度
/// @param[in] testLon 点的经度
/// @return 点是否在多边形内
bool Processor::pnpoly(const std::vector<std::pair<float,float>> &polygon, float testLat, float testLon) {
    bool isInPolygon = false;
    size_t i, j;
    /// 多边形的顶点个数
    size_t nvert = polygon.size();
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((polygon[i].first > testLat) != (polygon[j].first > testLat)) && (testLon < (polygon[j].second - polygon[i].second) * (testLat - polygon[i].first) / (polygon[j].first - polygon[i].first) + polygon[i].second))
            isInPolygon = !isInPolygon;
    }
    return isInPolygon;
}

/// @brief 检查点是否在任一个多边形
/// @param[in] testLat 点的纬度
/// @param[in] testLon 点的经度
/// @return 点是否在任一个多边形
bool Processor::pnpolys(float testLat, float testLon) {
    for (const auto& landPolygon : landPolygons) {
        if (pnpoly(landPolygon, testLat, testLon)) { return true; }
    }
    return false;
}

}
