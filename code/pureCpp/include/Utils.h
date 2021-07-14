#pragma once

#include <ctime>
#include <string>
#include <netcdf>
#include <utility>
#include <vector>
#include <unordered_set>

namespace Constants {
    /// 一天中最多会出现的涡旋数量
    constexpr int TODAY_MAX_TP_NUM = 5;
    /// 查看相对涡度场时认为可能有台风的最小相对涡度值
    constexpr float HAS_TP_MIN_ReVOR = 2e-5;
    /// 递归找出“台风”包含的所有空间点，如果点数少于这个值则认为这不是台风
    constexpr int TP_MIN_PTS = 5;
    /// 认为这个是台风的最大偏心率（偏心率计算方法仍需改进）
    constexpr float TP_MIN_E = 0.9;
    /// 找出所有点的递归中限定相对涡度的最小值
    constexpr float RECURSION_MIN_ReVOR = 4e-5;
    /// 台风在一个时次内走的最远距离，超过此距离认为是两个不同的台风
    constexpr float LINK_TP_MAX_DIST = 300.0;

    const std::string VOR_FILE_DIR = "/mnt/e/University/TC_Tracker/data/";
    const std::string VOR_FILE_PATH = VOR_FILE_DIR + "Vorticity_JRA-55_hourly.nc";

    constexpr int latGridNum = 41, lonGridNum = 89;
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
    void getLatLonData(netCDF::NcFile *iFile, float lat[Constants::latGridNum], float lon[Constants::latGridNum]);
    void getLatLonData(netCDF::NcFile *iFile, std::vector<float> &latVec, std::vector<float> &lonVec);
    void getVorField(netCDF::NcFile *iFile, float *vor);
    std::pair<std::pair<int, int>, float> max_element_2d(float vorField[Constants::latGridNum][Constants::lonGridNum]);

    std::pair<float, float> getVortexCenterLatLon(const std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex, float latArray[Constants::latGridNum], float lonArray[Constants::lonGridNum]);
    float cellDist(float lat[Constants::latGridNum], float lon[Constants::latGridNum], std::pair<int, int> cell1Index, std::pair<int, int> cell2Index);

}


