
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
#include <boost/serialization/vector.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <omp.h>
#include "json.hpp"
#include "multiArray.h"
#include "Processor.h"
#include "Utils.h"
#include "Typhoon.h"


namespace TTCore {

Processor::Processor(bool* isCanceled, netCDF::NcFile &iFile, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, int threadNum, const std::string& dumpDirectory) : isCanceled(isCanceled), isWrfoutFile(isWrfoutFile), varNames(varNames), zLevelIndex(zLevelIndex), threadNum(threadNum), dumpDir(dumpDirectory), iiFile(&iFile), tcInfo(getTCInfo()) {
    
    threadNum ? threadNum : omp_get_max_threads();
    getDimLength();
    allVortexes = std::vector<std::vector<TC1Time>>(timeLength);
    if (isWrfoutFile) {
        latArr2D = TwoDArray(latGridNum, lonGridNum);
        lonArr2D = TwoDArray(latGridNum, lonGridNum);
        UtilFunc::getLatLonData2d(iiFile, latGridNum, lonGridNum, latArr2D.get(), lonArr2D.get());
    } else {
        latArr = std::make_unique<float[]>(latGridNum);
        lonArr = std::make_unique<float[]>(lonGridNum);
        UtilFunc::getLatLonData(iiFile, varNames.latVarName, varNames.lonVarName, latArr.get(), lonArr.get());
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
    std::string timeUnits;
    auto timeVar = iiFile->getVar(varNames.timeVarName);
    timeVar.getAtt("units").getValues(timeUnits);
    double firstTValue;
    timeVar.getVar({0}, {1}, &firstTValue);
    return TCInfo(timeUnits, UtilFunc::getTimeInterval(timeVar), firstTValue);
    //        vortexes = Vortexes(timeUnits, UtilFunc::getTimeInterval(timeVar));
//    return Vortexes(timeUnits, UtilFunc::getTimeInterval(timeVar));
}

/// 此方法找出文件的各维度的长度
void Processor::getDimLength() {
    if (!isWrfoutFile) {
        auto vorVar = iiFile->getVar(varNames.vorVarName);
        timeLength = vorVar.getDim(0).getSize();
        latGridNum = vorVar.getDim(1).getSize();
        lonGridNum = vorVar.getDim(2).getSize();
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
void Processor::recognizeTyphoon() {
    std::cout << "第一步(recognize_typhoon): 导入文件成功，开始识别。" << std::endl;
    
    /// 记录前一个时次的台风数目
//    int TCNum_prevTime = 0;
    //constexpr int startYear = 1979, endYear = 2018;
    
    /// startYear的1月1日0时时次在文件中的index
//    int startIndexInFile = 0;
    /// endYear的12月31日0时时次在文件中的index
    //        int endIndexInFile = 58436;
    
    auto vorField = ThreeDArray(timeLength, latGridNum, lonGridNum);
    
    if (isWrfoutFile) {
        calcRelativeVorField(iiFile, vorField);
    } else {
        iiFile->getVar(varNames.vorVarName).getVar(vorField.get());
    }
    Constants::RECURSION_MIN_ReVOR = std::abs(vorField.avgMinValue(threadNum));
    //Constants::HAS_TP_MIN_ReVOR = isWrfoutFile ? 100e-5 : 8e-5;
    std::cout << "RECURSION_MIN_ReVOR: " << Constants::RECURSION_MIN_ReVOR << std::endl;
    
    
//    unsigned long itsPerCheck = timeLength / 20;
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
        
        getVortexNum1Time(vorField, timeIndex);
//        TCNum_prevTime = tpNum_timei;
        
    }
    std::cout << "Done step1" << std::endl;
}

/// 第二步：跟踪第一步生成的每个时次的气旋，生成真正的气旋对象。
void Processor::getRealTC() {
    std::cout << "开始跟踪" << std::endl;
    UtilFunc::modifyMaxDist(iiFile, isWrfoutFile ? "XTIME" : varNames.timeVarName);
    
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
                        i+TC_No, {currentTimeVortexes[i].maxVorCellIndex}, {currentTimeVortexes[i].geoCenter}, timeIndex, timeIndex
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
                            TC_No+i, {currentTimeVortexes[i].maxVorCellIndex}, {currentTimeVortexes[i].geoCenter}, timeIndex, timeIndex
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
}

/// 此方法识别某个时次是否有台风以及台风的个数
/// @param[in] vorField 涡度场（时间、纬度、经度）
/// @param[in] timeIndex 时间index
/// @return 当前时次的涡旋数量
int Processor::getVortexNum1Time(ThreeDArray &vorField, int timeIndex) {
    /// 当前时次的涡旋的个数
    int tpNum = 0;
    std::vector<TC1Time> vortexesThisTime{};
    std::vector<std::unordered_set<std::pair<int, int>, pair_hash>> vortexesCellsIndex;
    
    for (int i = 0; i < Constants::TODAY_MAX_TP_NUM; ++i) {
        // auto maxVorCell = UtilFunc::max_element_2d(vorField);
        auto maxVorCell = vorField.max(timeIndex);
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
        auto vortexCenterLatLon = isWrfoutFile ? UtilFunc::getVortexCenterLatLon(allCellsIndex, latArr2D, lonArr2D) : UtilFunc::getVortexCenterLatLon(allCellsIndex, latArr.get(), lonArr.get());
        vortexesCellsIndex.push_back(allCellsIndex);
        vortexesThisTime.push_back(TC1Time{maxVorCell.first, vortexCenterLatLon});
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
/// @param[in] vorField 涡度场（2d array）
/// @param[in] maxValIndex 涡度最大值的格点对应的纬度、经度index
/// @return 周围的点的index
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
/// @param[out] vortexCellsIndex 涡旋包含的点的index
inline void Processor::removeVortex(ThreeDArray &vorField, int timeIndex, std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex) {
    for (auto &i : vortexCellsIndex) {
        vorField(timeIndex, i.first, i.second) = 1e-6;
    }
}

int Processor::getLastNotEmptyVecIndex() {
    int i = allVortexes.size() - 1;
    while (allVortexes[i].empty()) { --i; }
    return i;
}


/// @brief 此方法检查一个文件夹名字对应的路径是否为路径
/// @param folderName 文件夹名称
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

void Processor::dumpStep3(const std::string ncFilePath) {
    //checkDirAndCreate("step3");
    std::filesystem::path stepDumpDir(dumpDir);
    std::ofstream ofs(stepDumpDir / (std::filesystem::path(ncFilePath).stem().string() + "_step3.dat"), std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    TCs tcs(realTCs, tcInfo);
    oa << tcs;
}

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

void Processor::getStep2DataFromFile(const std::string& filePath) {
    std::ifstream ifs(filePath, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);
    
    realTCs.clear();
    TCs tcs;
    ia >> tcs;
    realTCs = tcs.getTcs();
    tcInfo = tcs.getTcInfo();

}

void Processor::copyRealTCs(std::vector<Typhoon>& tcs) {
    tcs = realTCs;
    std::cout << "msg from copy, realTCs number: " << tcs.size() << std::endl;
    std::cout << "copy tcs completed." << std::endl;
}

void Processor::copyTCs(TCs &tcs) {
    tcs = TCs(realTCs, tcInfo);
}

void Processor::getLandPolygons() {
    auto exeDirStr = boost::dll::program_location().parent_path().string();
    std::filesystem::path exeDir(exeDirStr);
    std::ifstream ifs(exeDir / "data" / "myMap.dat", std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);
    ia >> landPolygons;
}


/// @brief 检查点是否在多边形里
/// @param[in] polygon 多边形
/// @param[in] testLat 点的纬度
/// @param[in] testLon 点的经度
/// @return 点是否在多边形内
bool Processor::pnpoly(const std::vector<std::pair<float,float>> &polygon, float testLat, float testLon) {
    bool isInPolygon = false;
    int i, j;
    /// 多边形的顶点个数
    int nvert = polygon.size();
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

