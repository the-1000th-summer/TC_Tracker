
#include <iostream>
#include <iterator>
#include <algorithm>
#include <netcdf>
#include <ostream>
#include <utility>
#include <vector>
#include <unordered_set>
#include "Processor.h"
#include "Utils.h"
#include "Typhoon.h"

Processor::Processor(netCDF::NcFile &iFile) {
    iiFile = &iFile;
    UtilFunc::getLatLonData(iiFile, latArr, lonArr);
    // std::cout << a << std::endl;
    // iFile = netCDF::NcFile("/mnt/e/University/TC_Tracker/data/Vorticity_JRA-55_hourly.nc", netCDF::NcFile::read)
}


/// 第一步：找出有台风的日期，记录日期与台风信息
void Processor::recognizeTyphoon(float vorField[Constants::latGridNum][Constants::lonGridNum]) {
    std::cout << "第一步(recognize_typhoon): 导入文件成功，开始识别。" << std::endl;

    /// 记录前一个时次的台风数目
    int tpNum_prevTime = 0;
    constexpr int startYear = 1979, endYear = 2018;

    /// startYear的1月1日0时时次在文件中的index
    int startIndexInFile = 0;
    /// endYear的12月31日0时时次在文件中的index
    int endIndexInFile = 58436;

    for (int timeIndex = startIndexInFile; timeIndex <= endIndexInFile; ++timeIndex) {
        int tpNum_timei = getVortexNum1Time(vorField, tpNum_prevTime);
        tpNum_prevTime = tpNum_timei;
        if (tpNum_timei >= 1) {
            hasTC_timeIndex.push_back(timeIndex);
        }
    }

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
    std::vector<Typhoon> temp_TCs{}, real_TCs{};

    while (timeIndex <= hasTCLastTimeIndex) {
        hasTCCurrentTime = (std::find(hasTC_timeIndex.begin(), hasTC_timeIndex.end(), timeIndex) != hasTC_timeIndex.end()) ? true : false;
        if (!hasTCCurrentTime) {
            if (hasTCPrevTime) {  // 当前时次无气旋，前一时次有气旋，确定气旋消亡日期
                int tempTCSize = temp_TCs.size();
                for (int i = 0; i < tempTCSize; ++i) {
                    temp_TCs[i].endTimeIndex = timeIndex - 1;
                }
                real_TCs.insert(real_TCs.end(), temp_TCs.begin(), temp_TCs.end());
                temp_TCs.clear();
            }

        } else {
            auto currentTimeVortexes = allVortexes[hasVortex_notHandledYetIndex];
            int currentTimeTCNum = currentTimeVortexes.size();
            ++hasVortex_notHandledYetIndex;
            if (!hasTCPrevTime) {  // 当前时次有气旋，但前一时次无气旋，创建新的气旋对象
                
                for (int i = 0; i < currentTimeTCNum; ++i) {
                    temp_TCs.push_back(Typhoon{
                        i+TC_No, {currentTimeVortexes[i].maxVorCellIndex}, timeIndex, timeIndex
                    });
                }
                // 更新气旋编号，注意更新后的编号所代表的气旋仍未出现
                TC_No += currentTimeTCNum;
            } else {             // 当前时次和前一时次都有气旋
                
            }
        }
        hasTCPrevTime = hasTCCurrentTime;
        ++timeIndex;
    }

}

/// 此方法识别某个时次是否有台风以及台风的个数
/// @param[in] vorField 涡度场（2d array）
/// @return 当前时次的涡旋数量
int Processor::getVortexNum1Time(float vorField[Constants::latGridNum][Constants::lonGridNum], int tpNum_prevTime) {
    /// 当前时次的涡旋的个数
    int tpNum = 0;
    std::vector<TC1Time> vortexesThisTime{};

    for (int i = 0; i < Constants::TODAY_MAX_TP_NUM; ++i) {
        auto maxVorCell = UtilFunc::max_element_2d(vorField);
        std::unordered_set<std::pair<int, int>, pair_hash> allCellsIndex;
        getVortexCellsIndex(vorField, maxVorCell.first, allCellsIndex);
        // auto a = allCells.begin();
        // while (a != allCells.end()) {
        //     std::cout << a->first << "," << a->second << std::endl;
        //     ++a;
        // }
        if (allCellsIndex.empty())
            break;
        if ((allCellsIndex.size() <= Constants::TP_MIN_PTS) && (tpNum_prevTime == 0) )
            break;

        /// @todo 偏心率的计算逻辑
        float e;

        ++tpNum;
        auto vortexCenterLatLon = UtilFunc::getVortexCenterLatLon(allCellsIndex, latArr, lonArr);
        vortexesThisTime.push_back(TC1Time{maxVorCell.first});
        removeVortex(vorField, allCellsIndex);
    }
    allVortexes.push_back(vortexesThisTime);
    return tpNum;

}

/// 此函数接受一个点，递归返回所有在台风内的点（阈值采用相对涡度）
/// @param[in] vorField 涡度场（2d array）
/// @param[in] maxValIndex 涡度最大值的格点对应的纬度、经度index
void Processor::getVortexCellsIndex(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCellsIndex) {
    if (vorField[maxValIndex.first][maxValIndex.second] >= Constants::RECURSION_MIN_ReVOR) {
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
std::unordered_set<std::pair<int, int>, pair_hash> Processor::getSurroundingCellsIndex(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> cellIndex) {
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
    }
    // 点在右边缘
    if (cellIndex.second == Constants::lonGridNum - 1) {
        std::vector<std::pair<int, int>> excludeCells{{cellIndex.first+1, cellIndex.second+1}, {cellIndex.first, cellIndex.second+1}, {cellIndex.first-1, cellIndex.second+1}};
        for (auto &i : excludeCells) {
            surroundingCells.erase(i);
        }
    }
    // 点在上边缘
    if (cellIndex.first == Constants::latGridNum - 1) {
        std::vector<std::pair<int, int>> excludeCells{{cellIndex.first+1, cellIndex.second-1}, {cellIndex.first+1, cellIndex.second}, {cellIndex.first+1, cellIndex.second+1}};
        for (auto &i : excludeCells) {
            surroundingCells.erase(i);
        }
    }
    // 点在下边缘
    if (cellIndex.first == 0) {
        std::vector<std::pair<int, int>> excludeCells{{cellIndex.first-1, cellIndex.second-1}, {cellIndex.first-1, cellIndex.second}, {cellIndex.first-1, cellIndex.second+1}};
        for (auto &i : excludeCells) {
            surroundingCells.erase(i);
        }
    }
    return surroundingCells;
}

/// 此函数消除相对涡度大值中心，为识别多个台风服务，替换为1e-6
/// @param[inout] vorField 涡度场（2d array）
/// @param[vortexCellsIndex] 涡旋包含的点的index
void Processor::removeVortex(float vorField[Constants::latGridNum][Constants::lonGridNum], std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex) {
    for (auto &i : vortexCellsIndex) {
        vorField[i.first][i.second] = 1e-6;
    }
}
