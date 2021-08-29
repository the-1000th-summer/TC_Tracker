#pragma once

#include <cmath>
#include <ctime>
#include <string>
#include <netcdf>
#include <utility>
#include <vector>
#include <unordered_set>
#include <numeric>

#include "multiArray.h"

namespace TTCore {

    namespace Constants {
        /// 一天中最多会出现的涡旋数量
        constexpr int TODAY_MAX_TP_NUM = 5;
        /// 查看相对涡度场时认为可能有台风的最小相对涡度值
        extern float HAS_TP_MIN_ReVOR;
        //constexpr float HAS_TP_MIN_ReVOR = 8e-5;
        /// 递归找出“台风”包含的所有空间点，如果点数少于这个值则认为这不是台风
        constexpr int TP_MIN_PTS = 4;
        /// 认为这个是台风的最大偏心率（偏心率计算方法仍需改进）
        constexpr float TP_MIN_E = 0.9;
        /// 找出所有点的递归中限定相对涡度的最小值
        extern float RECURSION_MIN_ReVOR;
        //constexpr float RECURSION_MIN_ReVOR = 30e-5;
        //constexpr float RECURSION_MIN_ReVOR = 6e-5;
        /// 台风在一个时次内走的最远距离，超过此距离认为是两个不同的台风
        extern double LINK_TP_MAX_DIST;

        //const std::string VOR_FILE_DIR = "/mnt/e/University/TC_Tracker/data/";
        //const std::string VOR_FILE_PATH = VOR_FILE_DIR + "Vorticity_JRA-55_hourly.nc";

        // constexpr int latGridNum = 41, lonGridNum = 89;
    }

    struct pair_hash {
        inline std::size_t operator()(const std::pair<int,int> & v) const {
            return v.first*31+v.second;
        }
    };

    namespace UtilFunc {
        bool ifFileExists (const std::string& name);
        std::time_t getEpochTime(const std::string& dateTime, const std::string& dateTimeFormat="%Y-%m-%dT%H:%M:%SZ");
        void getTimeData(netCDF::NcFile &iFile);
        void num2Date(double timeNum, tm &dateTimeTm, std::string timeUnits);
        void num2Date(double timeNum[], std::string timeUnits);
        void getLatLonData(netCDF::NcFile *iFile, const std::string& latVarName, const std::string& lonVarName, float *latArray, float *lonArray);
        void getLatLonData(netCDF::NcFile *iFile, const std::string& latVarName, const std::string& lonVarName, std::vector<float> &latVec, std::vector<float> &lonVec);
        void getLatLonData2d(netCDF::NcFile *iFile, unsigned long latGridNum, unsigned long lonGridNum, float* latArray2d, float* lonArray2d);
        void getVorField(netCDF::NcFile *iFile, float *vor);
        void modifyMaxDist(netCDF::NcFile *iFile, const std::string &timeVarName);

        // std::pair<std::pair<int, int>, float> max_element_2d(float vorField[Constants::latGridNum][Constants::lonGridNum]);
        // template <int latGridNum, int lonGridNum>
        // std::pair<std::pair<int, int>, float> max_element_2d(float (&vorField)[latGridNum][lonGridNum]);
        // template <int latGridNum, int lonGridNum>
        // std::pair<std::pair<int, int>, float> minn_element_2d(float (&vorField)[latGridNum][lonGridNum]);

        // std::pair<std::pair<int, int>, float> min_element_2d(float **vorField, int latGridNum, int lonGridNum);


        std::pair<float, float> getVortexCenterLatLon(const std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex, float *latArray, float *lonArray);
        std::pair<float, float> getVortexCenterLatLon(const std::unordered_set<std::pair<int, int>, pair_hash>& vortexCellsIndex, TwoDArray &latArray, TwoDArray &lonArray);

        float cellDist(float *latArray, float *lonArray, std::pair<int, int> cell1Index, std::pair<int, int> cell2Index);
        float cellDist(float lat1, float lon1, float lat2, float lon2);

        std::pair<std::pair<int, int>, float> getMaxDistance(std::vector<std::pair<int, int>> &cellsIndex);
    

        inline float euclideanDist2(const std::pair<float, float> &cell1Index, const std::pair<float, float> &cell2Index);
        inline float getSlope(const std::pair<float, float> &cell1Index, const std::pair<float, float> &cell2Index);
    
    
        std::pair<float, float> getCellsCenterLatLon(const std::pair<int, int> &cell1Index, const std::pair<int, int> &cell2Index, const float *latArray, const float *lonArray);

        bool alwaysMoveEast(const std::vector<std::pair<int, int>> &cells);
        float cellsLatAvg(const float *latArray, const std::vector<std::pair<int, int>> &cells);
        inline float cellsLatAvg(const std::vector<std::pair<float, float>>& geoCenters);
        float cellsLonAvg(const float* lonArray, const std::vector<std::pair<int, int>>& cells);
        inline float cellsLonAvg(const std::vector<std::pair<float, float>>& geoCenters);
    }

    /// 计算两点欧氏距离
    inline float UtilFunc::euclideanDist2(const std::pair<float, float> &cell1Index, const std::pair<float, float> &cell2Index) {
        return std::pow(cell1Index.first-cell2Index.first, 2) + std::pow(cell1Index.second-cell2Index.second, 2);
    }

    /// 计算两点斜率
    inline float UtilFunc::getSlope(const std::pair<float, float> &cell1Index, const std::pair<float, float> &cell2Index) {
        return (cell2Index.first == cell1Index.first) ? std::numeric_limits<float>::infinity() : (cell2Index.second - cell1Index.second) / (cell2Index.first - cell1Index.first);
    }

    inline float UtilFunc::cellsLatAvg(const std::vector<std::pair<float, float>>& geoCenters) {
        return std::accumulate(geoCenters.begin(), geoCenters.end(), 0.0f, [](auto& a, auto& b) {return a + b.first; }) / geoCenters.size();
    }
    inline float UtilFunc::cellsLonAvg(const std::vector<std::pair<float, float>>& geoCenters) {
        return std::accumulate(geoCenters.begin(), geoCenters.end(), 0.0f, [](auto& a, auto& b) {return a + b.second; }) / geoCenters.size();
    }
}
