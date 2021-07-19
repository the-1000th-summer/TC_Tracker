#pragma once

#include <netcdf>
#include <string>
#include <utility>
#include "Utils.h"
#include <unordered_set>
#include <vector>
#include "multiArray.h"
#include "TC1Time.h"


class Processor {
public:
    Processor(netCDF::NcFile &iFile);
    ~Processor();
    void getDimLength();
    void recognizeTyphoon();
    void getRealTC();
    void removeNoise();
    int getVortexNum1Time(ThreeDArray &vorField, int timeIndex, int TCNum_prevTime);
    void getVortexCellsIndex(ThreeDArray &vorField, int timeIndex, std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCells);
    std::unordered_set<std::pair<int, int>, pair_hash> getSurroundingCellsIndex(std::pair<int, int> cellIndex);
    float get_e(std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex);
    std::pair<float, float> getMinorAxisLen(const std::vector<std::pair<int, int>> &cellsIndex, const std::pair<float, float> &centerLatLon, float minorAxisK, const float gridRatio, const float A);

    inline void removeVortex(ThreeDArray &vorField, int timeIndex, std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex);
    
    
private:
    netCDF::NcFile *iiFile;
    unsigned long timeLength = 0;
    unsigned long latGridNum = 0;
    unsigned long lonGridNum = 0;
    /// 记录所有有台风的日期的数组（从基准日期起算的时次数）
    std::unique_ptr<float[]> latArr, lonArr;
    std::vector<int> hasTC_timeIndex{};
    std::vector<std::vector<TC1Time>> allVortexes{};
};


