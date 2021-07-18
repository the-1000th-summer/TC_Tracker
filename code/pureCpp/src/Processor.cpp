
#include <cmath>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <netcdf>
#include <ostream>
#include <utility>
#include <vector>
#include <unordered_set>
#include "multiArray.h"
#include "Processor.h"
#include "Utils.h"
#include "Typhoon.h"

Processor::Processor(netCDF::NcFile &iFile) {
    iiFile = &iFile;
    getDimLength();
    latArr = std::make_unique<float[]>(latGridNum);
    lonArr = std::make_unique<float[]>(lonGridNum);
    UtilFunc::getLatLonData(iiFile, latArr.get(), lonArr.get());
    // std::cout << a << std::endl;
    // iFile = netCDF::NcFile("/mnt/e/University/TC_Tracker/data/Vorticity_JRA-55_hourly.nc", netCDF::NcFile::read)
}

Processor::~Processor() {
    iiFile->close();
    iiFile = nullptr;
}


/// 此方法找出文件的各维度的长度
void Processor::getDimLength() {
    auto vorVar = iiFile->getVar("Vorticity");
    timeLength = vorVar.getDim(0).getSize();
    latGridNum = vorVar.getDim(1).getSize();
    lonGridNum = vorVar.getDim(2).getSize();
}


/// 第一步：找出有台风的日期，记录日期与台风信息
void Processor::recognizeTyphoon() {
    std::cout << "第一步(recognize_typhoon): 导入文件成功，开始识别。" << std::endl;

    /// 记录前一个时次的台风数目
    int TCNum_prevTime = 0;
    constexpr int startYear = 1979, endYear = 2018;

    /// startYear的1月1日0时时次在文件中的index
    int startIndexInFile = 0;
    /// endYear的12月31日0时时次在文件中的index
    int endIndexInFile = 58436;

    auto vorVar = iiFile->getVar("Vorticity");
    
    // float vorField[latGridNum][lonGridNum];
    auto vorField = TwoDArray(latGridNum, lonGridNum);

    // float (*arrayy)[Constants::latGridNum][Constants::lonGridNum] = new float[58440][Constants::latGridNum][Constants::lonGridNum];
    // vorVar.getVar(arrayy);

    for (unsigned long timeIndex = startIndexInFile; timeIndex <= endIndexInFile; ++timeIndex) {
        // std::cout << vorVar.getName() << std::endl;
        if (timeIndex % 1000 == 0)
            std::cout << timeIndex << std::endl; 
        vorVar.getVar({timeIndex,0,0}, {1, latGridNum, lonGridNum}, vorField.get());

        int tpNum_timei = getVortexNum1Time(vorField, TCNum_prevTime);
        TCNum_prevTime = tpNum_timei;
        if (tpNum_timei >= 1) {
            hasTC_timeIndex.push_back(timeIndex);
        }
        
    }
    // delete [] arrayy;
    std::cout << "Done step1" << std::endl;
}

/// 第二步：跟踪第一步生成的每个时次的气旋，生成真正的气旋对象。
void Processor::getRealTC() {
    std::cout << "开始跟踪" << std::endl;

    /// 当前时次和前一时次是否有气旋
    bool hasTCCurrentTime = false, hasTCPrevTime = false;
    /// 气旋编号，从1开始
    int TC_No = 1;
    /// 当前时次的index
    int timeIndex = 0;
    ///
    int hasVortex_notHandledYetIndex;
    /// 最后有气旋的时次的index
    int hasTCLastTimeIndex = hasTC_timeIndex.back();
    /// 跟踪时仍未确定消亡日期的气旋；跟踪完成的气旋
    std::vector<Typhoon> tempTCs{}, real_TCs{};

    while (timeIndex <= hasTCLastTimeIndex) {
        hasTCCurrentTime = (std::find(hasTC_timeIndex.begin(), hasTC_timeIndex.end(), timeIndex) != hasTC_timeIndex.end()) ? true : false;
        if (!hasTCCurrentTime) {
            if (hasTCPrevTime) {  // 当前时次无气旋，前一时次有气旋，确定气旋消亡日期
                int tempTCSize = tempTCs.size();
                for (int i = 0; i < tempTCSize; ++i) {
                    tempTCs[i].endTimeIndex = timeIndex - 1;
                }
                real_TCs.insert(real_TCs.end(), tempTCs.begin(), tempTCs.end());
                tempTCs.clear();
            }

        } else {
            /// 当前时次的涡旋vector
            auto currentTimeVortexes = allVortexes[hasVortex_notHandledYetIndex];
            /// 当前时次的涡旋个数
            int currentTimeTCNum = currentTimeVortexes.size();
            ++hasVortex_notHandledYetIndex;
            if (!hasTCPrevTime) {  // 当前时次有气旋，但前一时次无气旋，创建新的气旋对象
                
                for (int i = 0; i < currentTimeTCNum; ++i) {
                    tempTCs.push_back(Typhoon{
                        i+TC_No, {currentTimeVortexes[i].maxVorCellIndex}, timeIndex, timeIndex
                    });
                }
                // 更新气旋编号，注意更新后的编号所代表的气旋仍未出现
                TC_No += currentTimeTCNum;
            } else {             // 当前时次和前一时次都有气旋
                /// 当前时次的涡旋index
                std::vector<TC1Time> currentTVortexes_copy(currentTimeVortexes);
                // std::vector<int> currentT_TCsIndex(currentTimeVortexes.size());
                // std::iota(currentT_TCsIndex.begin(), currentT_TCsIndex.end(), 0);

                /// 正在跟踪的气旋index
                std::vector<Typhoon> tempTCS_copy(tempTCs);
                // std::vector<int> tempTCsIndex(tempTCs.size());
                // std::iota(tempTCsIndex.begin(), tempTCsIndex.end(), 0);

                /// 正在跟踪的气旋index，用于判断每个台风是否被处理
                std::vector<Typhoon> tempTCS_ForD(tempTCs);
                // std::vector<int> tempTCsIndexForD(tempTCs.size());
                // std::iota(tempTCsIndexForD.begin(), tempTCsIndexForD.end(), 0);


                std::vector<std::pair<int, int>> temp_TCs_max_pt, currentT_TCs_maxVorCellIndex;
                std::transform(tempTCs.begin(), tempTCs.end(), std::back_inserter(temp_TCs_max_pt), [](const Typhoon &temp_TC) {return temp_TC.maxVorCells.back();});
                std::transform(currentTimeVortexes.begin(), currentTimeVortexes.end(), std::back_inserter(currentT_TCs_maxVorCellIndex), [](const TC1Time &currentTimeVortex) {return currentTimeVortex.maxVorCellIndex;});

                int tempTCsSize = tempTCs.size();
                int currentTVortexesSize = currentTimeVortexes.size();
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

                while (!tempTCS_ForD.empty()) {
                    if (currentTVortexes_copy.empty()) {
                        // 如果进入了这个条件，就说明当前时次的气旋比昨天的少，当前时次的气旋都已经对应成功
                        // 现在处理的是上一个时次的没有对应的气旋
                        auto willBeRMTC = tempTCS_ForD[0];
                        real_TCs.push_back(willBeRMTC);
                        // tempTCsIndex.erase(std::remove(tempTCsIndex.begin(), tempTCsIndex.end(), willBeRMTCIndex), tempTCsIndex.end());
                        // tempTCsIndexForD.erase(std::remove(tempTCsIndexForD.begin(), tempTCsIndexForD.end(), willBeRMTCIndex), tempTCsIndexForD.end());
                        // tempTCS_copy.erase(std::remove(tempTCS_copy.begin(), tempTCS_copy.end(), willBeRMTC));
                        // tempTCS_ForD.erase(std::remove(tempTCS_ForD.begin(), tempTCS_ForD.end(), willBeRMTC));
                        continue;
                    }
                    // 寻找两个靠的最近的昨日台风与今日台风
                    // auto minDist = UtilFunc::min_element_2d(distMatrix, tempTCsSize, currentTVortexesSize);
                    auto minDist = distMatrix.min();
                    auto minDistIndex = minDist.first;
                    auto minD_tempTC = tempTCs[minDistIndex.first];
                    auto minD_currentTTC = currentTimeVortexes[minDistIndex.second];

                    if (minDist.second > Constants::LINK_TP_MAX_DIST) {  // 该台风昨天就消亡了
                        real_TCs.push_back(minD_tempTC);
                        // 从临时数组中移去已消亡的台风
                        // tempTCsIndex.erase(std::remove(tempTCsIndex.begin(), tempTCsIndex.end(), minDistIndex.first), tempTCsIndex.end());
                        // tempTCsIndexForD.erase(std::remove(tempTCsIndexForD.begin(), tempTCsIndexForD.end(), minDistIndex.first), tempTCsIndexForD.end());
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
                        // 更新最大风速
                        //
                        // 更新台风长轴中心移动到的位置
                        //
                        // 更新台风四点中心移动到的位置
                        //
                        // 更新台风几何中心移动到的位置
                        //

                        // tempTCS_ForD.erase(std::remove(tempTCS_ForD.begin(), tempTCS_ForD.end(), minD_tempTC));
                        // 移除对应成功的当前时次的涡旋
                        // currentTVortexes_copy.erase(std::remove(currentTVortexes_copy.begin(), currentTVortexes_copy.end(), minD_currentTTC));

                        for (int i = 0; i < currentTVortexesSize; ++i)
                            distMatrix(minDistIndex.first, i) = 1e5;
                        for (int i = 0; i < tempTCsSize; ++i)
                            distMatrix(i, minDistIndex.second) = 1e5;
                    }

                    tempTCs = tempTCS_copy;

                    // 前一时次的所有的气旋对应完，today_tps_copy剩下新生成的、未被跟踪的气旋
                    if (!currentTVortexes_copy.empty()) {
                        int currentTTCsNum = currentTVortexes_copy.size();
                        for (int i = 0; i < currentTTCsNum; ++i) {
                            tempTCs.push_back(Typhoon{
                                TC_No+i, {currentTVortexes_copy[i].maxVorCellIndex}, timeIndex, timeIndex
                            });
                        }
                        // 更新台风编号，注意更新后的编号所代表的台风仍未出现
                        TC_No += currentTTCsNum;
                    }
                }
            }
        }
        hasTCPrevTime = hasTCCurrentTime;
        ++timeIndex;
    }

}

/// 此方法识别某个时次是否有台风以及台风的个数
/// @param[in] vorField 涡度场（2d array）
/// @return 当前时次的涡旋数量
int Processor::getVortexNum1Time(TwoDArray &vorField, int TCNum_prevTime) {
    /// 当前时次的涡旋的个数
    int tpNum = 0;
    std::vector<TC1Time> vortexesThisTime{};

    for (int i = 0; i < Constants::TODAY_MAX_TP_NUM; ++i) {
        // auto maxVorCell = UtilFunc::max_element_2d(vorField);
        auto maxVorCell = vorField.max();
        std::unordered_set<std::pair<int, int>, pair_hash> allCellsIndex;
        getVortexCellsIndex(vorField, maxVorCell.first, allCellsIndex);
        // auto a = allCells.begin();
        // while (a != allCells.end()) {
        //     std::cout << a->first << "," << a->second << std::endl;
        //     ++a;
        // }
        if (allCellsIndex.empty())
            break;
        if ((allCellsIndex.size() <= Constants::TP_MIN_PTS) && (TCNum_prevTime == 0) )
            break;

        /// @todo 偏心率的计算逻辑
        float e = get_e(allCellsIndex);
        if ((e > Constants::TP_MIN_E) && (TCNum_prevTime == 0))
            break;

        ++tpNum;
        auto vortexCenterLatLon = UtilFunc::getVortexCenterLatLon(allCellsIndex, latArr.get(), lonArr.get());
        vortexesThisTime.push_back(TC1Time{maxVorCell.first});
        removeVortex(vorField, allCellsIndex);
    }
    allVortexes.push_back(vortexesThisTime);
    return tpNum;

}

/// 此函数接受一个点，递归返回所有在台风内的点（阈值采用相对涡度）
/// @param[in] vorField 涡度场（2d array）
/// @param[in] maxValIndex 涡度最大值的格点对应的纬度、经度index
void Processor::getVortexCellsIndex(TwoDArray &vorField, std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCellsIndex) {
    if (vorField(maxValIndex.first, maxValIndex.second) >= Constants::RECURSION_MIN_ReVOR) {
        if (!allCellsIndex.count(maxValIndex)) {
            allCellsIndex.insert(maxValIndex);
            auto surroundingCellsIndex = getSurroundingCellsIndex(vorField, maxValIndex);
            for (auto &cellIndex: surroundingCellsIndex) {
                getVortexCellsIndex(vorField, cellIndex, allCellsIndex);
            }
        }
    }

}

/// 此函数接受一个点，返回周围的点（最多8个），如果在边缘则排除边缘外的点。
/// @param[in] vorField 涡度场（2d array）
/// @param[in] maxValIndex 涡度最大值的格点对应的纬度、经度index
/// @return 周围的点的index
std::unordered_set<std::pair<int, int>, pair_hash> Processor::getSurroundingCellsIndex(TwoDArray &vorField, std::pair<int, int> cellIndex) {
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

float Processor::get_e(std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex, const float *latArray, const float *lonArray) {
    std::vector<std::pair<int, int>> vortexCellsIndex_v(vortexCellsIndex.begin(), vortexCellsIndex.end());
    auto distMax = UtilFunc::getMaxDistance(vortexCellsIndex_v);
    
    /// 半长轴长度（单位：度）
    float A = distMax.second * (latArray[1] - latArray[0]) / 2.0;
    /// 长轴斜率
    float majorAxisK = UtilFunc::getSlope(vortexCellsIndex_v[distMax.first.first], vortexCellsIndex_v[distMax.first.second]);
    /// 短轴斜率
    float minorAxisK = (majorAxisK == 0) ? std::numeric_limits<float>::infinity() : (-1 / majorAxisK);
    /// 椭圆中点纬度、经度
    auto centerLatLon = UtilFunc::getCellsCenterLatLon(vortexCellsIndex_v[distMax.first.first], vortexCellsIndex_v[distMax.first.second], latArray, lonArray);

    getMinorAxisLen(vortexCellsIndex_v, minorAxisK);
}

/// 此方法计算短轴长度
float Processor::getMinorAxisLen(const std::vector<std::pair<int, int>> &cellsIndex, const std::pair<float, float> &centerLatLon, float minorAxisK) {
    std::pair<float, float> cellsLatLon[cellsIndex.size()];
    std::transform(cellsIndex.begin(), cellsIndex.end(), cellsLatLon, [this](const std::pair<int, int> &cellIndex) -> std::pair<float, float> {
        return {latArr[cellIndex.first], lonArr[cellIndex.second]};
    });
    std::vector<std::pair<int, int>> validCellsIndex;
    /// 短轴轴线对应的弧度
    float minorAxisRad = std::atan(minorAxisK);
    for (int i = 0; i < cellsIndex.size(); ++i) {
        auto cellRad = std::atan(UtilFunc::getSlope(centerLatLon, cellsLatLon[i]));
        if (std::abs(minorAxisRad - cellRad) <= M_PI / 4)
            validCellsIndex.push_back(cellsIndex[i]);
    }
    if (validCellsIndex.size() <= 1)
        return 0.0;

    /// 取这些点中两个距离最远的点
    auto a = UtilFunc::getMaxDistance(validCellsIndex);
    /// 两个距离最远的点连线的弧度
    float appro_min_rad = std::atan(UtilFunc::getSlope(validCellsIndex[a.first.first], validCellsIndex[a.first.second]));
    /// 估计线与真实线夹角余弦
    float cosAlpha = std::cos(std::abs(appro_min_rad - minorAxisRad));
    if (cosAlpha < 0)               // 条带太窄导致两个点都在一边，
        return 0.0;                 // 可能使计算出的斜率严重出错（即与长轴有相同趋势）

    auto min_dists = a.second

    
}

/// 此函数消除相对涡度大值中心，为识别多个台风服务，替换为1e-6
/// @param[inout] vorField 涡度场（2d array）
/// @param[vortexCellsIndex] 涡旋包含的点的index
inline void Processor::removeVortex(TwoDArray &vorField, std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex) {
    for (auto &i : vortexCellsIndex) {
        vorField(i.first, i.second) = 1e-6;
    }
}
