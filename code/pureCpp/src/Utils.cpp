#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <math.h>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Utils.h"
#include "multiArray.h"
#include <netcdf>
#include <sys/stat.h>
#include <utility>
#include <vector>
#include <cmath>



bool UtilFunc::ifFileExists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

    
/// 将UTC时间字符串转换成UNIX时间戳（1970年1月1日00:00开始所经过的秒数）
/// @param[in] dataTime UTC时间字符串
/// @return UNIX时间戳
std::time_t UtilFunc::getEpochTime(const std::string& dateTime, const std::string& dateTimeFormat){
    // Let's consider we are getting all the input in
    // this format: '2014-07-25T20:17:22Z' (T denotes
    // start of Time part, Z denotes UTC zone).
    // A better approach would be to pass in the format as well.
    // static const std::wstring dateTimeFormat{ L"%Y-%m-%dT%H:%M:%SZ" };

    // Create a stream which we will use to parse the string,
    // which we provide to constructor of stream to fill the buffer.
    std::stringstream ss{ dateTime };
    std::tm dt{};
    // Now we read from buffer using get_time manipulator
    // and formatting the input appropriately.
    ss >> std::get_time(&dt, dateTimeFormat.c_str());
    if (ss.fail()) {
        throw std::runtime_error("parse time str failed.");
    }

    // mktime() interprets its input as local time
    // Minus timezone to get UTC time
    auto localtime = std::mktime(&dt);
    return localtime + dt.tm_gmtoff;
}

/// 将表示时间的数字转换成表示时间的tm结构
/// @param[in] timeNum 表示时间的数字
/// @param[out] dateTimeTm 表示时间的tm结构
/// @param[in] timeUnits 时间单位
void UtilFunc::num2Date(double timeNum, tm &dateTimeTm, std::string timeUnits) {
    std::string unitLength = timeUnits.substr(0, timeUnits.find(" "));
    if (unitLength == "hours") {
        auto refTime_t = UtilFunc::getEpochTime(timeUnits, "hours since %Y-%m-%d %H:%M");
        time_t dateTime_t = refTime_t + 60*60*timeNum;   // 有可能会产生截断
        dateTimeTm = *std::gmtime(&dateTime_t);
    } else {
        throw std::runtime_error("not implemented yet.");
    }
}

void UtilFunc::getTimeData(netCDF::NcFile &iFile) {
    auto timeVar = iFile.getVar("time");
    std::string timeUnits;
    timeVar.getAtt("units").getValues(timeUnits);
    std::cout << timeUnits << std::endl;
}

void UtilFunc::getLatLonData(netCDF::NcFile *iFile, float *latArray, float *lonArray) {
    iFile->getVar("lat").getVar(latArray);
    iFile->getVar("lon").getVar(lonArray);
}

void UtilFunc::getLatLonData(netCDF::NcFile *iFile, std::vector<float> &latVec, std::vector<float> &lonVec) {
    float lat[iFile->getDim("lat").getSize()], lon[iFile->getDim("lon").getSize()];
    iFile->getVar("lat").getVar(lat);
    iFile->getVar("lon").getVar(lon);
    latVec.assign(lat, lat + sizeof(lat) / sizeof(lat[0]) );
    lonVec.assign(lon, lon + sizeof(lon) / sizeof(lon[0]) );
}

/// 此函数将相对涡度从文件中提取出来
void UtilFunc::getVorField(netCDF::NcFile *iFile, float *vor) {
    iFile->getVar("Vorticity").getVar(vor);
}

/// 找出2d array的最大值和相应的index
/// @param[in] vorField 涡度场（2d array）
/// @return index（纬度index、经度index）和最大值
// std::pair<std::pair<int, int>, float> UtilFunc::max_element_2d(float vorField[Constants::latGridNum][Constants::lonGridNum]) {
//     /// 保存每行的最大值
//     float maxElements[Constants::latGridNum];
//     /// 保存每行的最大值对应的index
//     int maxElemLonIndexes[Constants::latGridNum];
//     for (int i = 0; i < Constants::latGridNum; ++i) {
//         auto rowMaxElemIter = std::max_element(vorField[i], vorField[i] + Constants::lonGridNum);
//         maxElements[i] = *rowMaxElemIter;
//         maxElemLonIndexes[i] = std::distance(vorField[i], rowMaxElemIter);
//     }
//     auto maxElemIter = std::max_element(maxElements, maxElements + Constants::latGridNum);
//     int maxElemLatIndex = std::distance(maxElements, maxElemIter);
//     return std::pair<std::pair<int, int>, float> {{maxElemLatIndex, maxElemLonIndexes[maxElemLatIndex]}, *maxElemIter};
// }

/// 找出2d array的最大值和相应的index
/// @param[in] vorField 涡度场（2d array）
/// @return index（纬度index、经度index）和最大值
// template <int latGridNum, int lonGridNum>
// std::pair<std::pair<int, int>, float> UtilFunc::max_element_2d(float (&vorField)[latGridNum][lonGridNum]) {
//     /// 保存每行的最大值
//     float maxElements[latGridNum];
//     /// 保存每行的最大值对应的index
//     int maxElemLonIndexes[latGridNum];
//     for (int i = 0; i < latGridNum; ++i) {
//         auto rowMaxElemIter = std::max_element(vorField[i], vorField[i] + lonGridNum);
//         maxElements[i] = *rowMaxElemIter;
//         maxElemLonIndexes[i] = std::distance(vorField[i], rowMaxElemIter);
//     }
//     auto maxElemIter = std::max_element(maxElements, maxElements + latGridNum);
//     int maxElemLatIndex = std::distance(maxElements, maxElemIter);
//     return std::pair<std::pair<int, int>, float> {{maxElemLatIndex, maxElemLonIndexes[maxElemLatIndex]}, *maxElemIter};
// }


// template <int latGridNum, int lonGridNum>
// std::pair<std::pair<int, int>, float> UtilFunc::minn_element_2d(float (&vorField)[latGridNum][lonGridNum]) {
//     /// 保存每行的最大值
//     float minElements[latGridNum];
//     /// 保存每行的最大值对应的index
//     int minElemLonIndexes[latGridNum];
//     for (int i = 0; i < latGridNum; ++i) {
//         auto rowMinElemIter = std::min_element(vorField[i], vorField[i] + lonGridNum);
//         minElements[i] = *rowMinElemIter;
//         minElemLonIndexes[i] = std::distance(vorField[i], rowMinElemIter);
//     }
//     auto minElemIter = std::min_element(minElements, minElements + latGridNum);
//     int minElemLatIndex = std::distance(minElements, minElemIter);
//     return std::pair<std::pair<int, int>, float> {{minElemLatIndex, minElemLonIndexes[minElemLatIndex]}, *minElemIter};
// }

/// 找出2d array的最小值和相应的index
/// @param[in] vorField 涡度场（2d array）
/// @param[in] latGridNum 行数
/// @param[in] lonGridNum 列数
/// @return index（纬度index、经度index）和最大值
// std::pair<std::pair<int, int>, float> UtilFunc::min_element_2d(float **vorField, int latGridNum, int lonGridNum) {
//     /// 保存每行的最大值
//     float minElements[latGridNum];
//     /// 保存每行的最大值对应的index
//     int minElemLonIndexes[latGridNum];
//     for (int i = 0; i < latGridNum; ++i) {
//         auto rowMinElemIter = std::min_element(vorField[i], vorField[i] + lonGridNum);
//         minElements[i] = *rowMinElemIter;
//         minElemLonIndexes[i] = std::distance(vorField[i], rowMinElemIter);
//     }
//     auto minElemIter = std::min_element(minElements, minElements + latGridNum);
//     int minElemLatIndex = std::distance(minElements, minElemIter);
//     return std::pair<std::pair<int, int>, float> {{minElemLatIndex, minElemLonIndexes[minElemLatIndex]}, *minElemIter};
// }

/// 此方法计算涡旋包含的点的中心位置
/// @param[in] vortexCellsIndex 涡旋包含的点的index
/// @param[in] latArray 纬度array
/// @param[in] lonArray 经度array
std::pair<float, float> UtilFunc::getVortexCenterLatLon(const std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex, float *latArray, float *lonArray) {
    float latAvg = 0, lonAvg = 0;
    for (auto &cellIndex : vortexCellsIndex) {
        latAvg += latArray[cellIndex.first];
        lonAvg += lonArray[cellIndex.second]; 
    }
    return {latAvg / vortexCellsIndex.size(), lonAvg / vortexCellsIndex.size()};
}

/// 此方法利用haversine公式计算两个点的真实距离(km)（大圆距离）
/// @param[in] latArray 纬度array
/// @param[in] lonArray 经度array
/// @param[in] cell1Index 第一个cell
/// @param[in] cell2Index 第二个cell
float UtilFunc::cellDist(float *latArray, float *lonArray, std::pair<int, int> cell1Index, std::pair<int, int> cell2Index) {
    // 两cell的纬度、经度（弧度形式）
    float lat1 = sin(latArray[cell1Index.first]*M_PI/180.0), lon1 = sin(lonArray[cell1Index.second]*M_PI/180.0);
    float lat2 = sin(latArray[cell2Index.first]*M_PI/180.0), lon2 = sin(lonArray[cell2Index.second]*M_PI/180.0);

    float dlat = lat2 - lat1, dlon = lon2 - lon1;
    float a = pow(sin(dlat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon/2), 2);
    float c = 2 * asin(sqrt(a));
    constexpr float r = 6371;
    return c * r;
}

/// 计算得到一组cell中距离最远的两个cell
std::pair<std::pair<int, int>, float> UtilFunc::getMaxDistance(std::vector<std::pair<int, int>> &cellsIndex) {
    auto distArray = TwoDArray(cellsIndex.size(), cellsIndex.size());
    int cellsNum = cellsIndex.size();
    for (int i = 0; i < cellsNum; ++i) {
        for (int j = 0; j < i; ++j) {
            distArray(i,j) = euclideanDist2(cellsIndex[i], cellsIndex[j]); 
        }
    }
    /// 长轴两个cell的index及它们之间的距离
    return distArray.max();
}




/// 计算两点欧氏距离
inline float UtilFunc::euclideanDist2(const std::pair<float, float> &cell1Index, const std::pair<float, float> &cell2Index) {
    return std::pow(cell1Index.first-cell2Index.first, 2) + std::pow(cell1Index.second-cell2Index.second, 2);
}

/// 计算两点斜率
inline float UtilFunc::getSlope(const std::pair<float, float> &cell1Index, const std::pair<float, float> &cell2Index) {
    return (cell2Index.first == cell1Index.first) ? std::numeric_limits<float>::infinity() : (cell2Index.second - cell1Index.second) / (cell2Index.first - cell1Index.first);
}

std::pair<float, float> UtilFunc::getCellsCenterLatLon(const std::pair<int, int> &cell1Index, const std::pair<int, int> &cell2Index, const float *latArray, const float *lonArray) {
    return {(latArray[cell1Index.first]+latArray[cell2Index.first])/2.0, (lonArray[cell1Index.second]+lonArray[cell2Index.second])/2.0};
}
