
#include "Processor.h"
#include "Utils.h"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <netcdf>
#include <ostream>
#include <utility>
#include <vector>
#include <unordered_set>

Processor::Processor(netCDF::NcFile &iFile) {
    iiFile = &iFile;
    // std::cout << a << std::endl;
    // iFile = netCDF::NcFile("/mnt/e/University/TC_Tracker/data/Vorticity_JRA-55_hourly.nc", netCDF::NcFile::read)
}

void Processor::a() {
    std::vector<float> latVec, lonVec;
    UtilFunc::getLatLonData(iiFile, latVec, lonVec);
    std::cout << "sdf" << std::endl;
}

void Processor::recognizeTyphoon(float vorField[Constants::latGridNum][Constants::lonGridNum]) {
    std::cout << "第一步(recognize_typhoon): 导入文件成功，开始识别。" << std::endl;

    /// 记录所有有台风的日期的数组（从基准日期起算的时次数）
    std::vector<int> hasTp_timeIndex{};
    /// 记录前一天的台风数目
    int tpNum_prevDay = 0;
    constexpr int startYear = 1979, endYear = 2018;

    /// startYear的1月1日0时时次在文件中的index
    int startIndexInFile;
    /// endYear的12月31日0时时次在文件中的index
    int endIndexInFile;

    for (int i = startIndexInFile; i <= endIndexInFile; ++i) {
        getVortexNum1Day(vorField);
    }

}

/// 此方法识别某天是否有台风以及台风的个数
/// @param[in] vorField 涡度场（2d array）
void Processor::getVortexNum1Day(float vorField[Constants::latGridNum][Constants::lonGridNum]) {
    /// 当天台风的个数
    int tpNum = 0;
    

    for (int i = 0; i < Constants::TODAY_MAX_TP_NUM; ++i) {
        auto maxVorCell = UtilFunc::max_element_2d(vorField);
        std::unordered_set<std::pair<int, int>, pair_hash> allCells;
        getVortexCells(vorField, maxVorCell.first, allCells);
        std::cout << "df" << std::endl;
    }
}

/// 此函数接受一个点，递归返回所有在台风内的点（阈值采用相对涡度）
/// @param[in] vorField 涡度场（2d array）
/// @param[in] maxValIndex 涡度最大值的格点对应的纬度、经度index
void Processor::getVortexCells(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCells) {
    if (vorField[maxValIndex.first][maxValIndex.second] >= Constants::RECURSION_MIN_ReVOR) {
        if (!allCells.count(maxValIndex)) {
            allCells.insert(maxValIndex);
            auto surroundingCells = getSurroundingCells(vorField, maxValIndex);
            for (auto &i: surroundingCells) {
                getVortexCells(vorField, i, allCells);
            }
        }
    }

}

/// 此函数接受一个点，返回周围的8个点，如果在边缘则排除边缘外的点。
/// @param[in] vorField 涡度场（2d array）
/// @param[in] maxValIndex 涡度最大值的格点对应的纬度、经度index
std::unordered_set<std::pair<int, int>, pair_hash> Processor::getSurroundingCells(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> cellIndex) {
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
