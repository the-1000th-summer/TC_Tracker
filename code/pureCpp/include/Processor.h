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
    int getVortexNum1Time(TwoDArray &vorField, int TCNum_prevTime);
    void getVortexCellsIndex(TwoDArray &vorField, std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCells);
    std::unordered_set<std::pair<int, int>, pair_hash> getSurroundingCellsIndex(TwoDArray &vorField, std::pair<int, int> cellIndex);
    float get_e(std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex, const float *latArray, const float *lonArray);
    float getMinorAxisLen(const std::vector<std::pair<int, int>> &cellsIndex, const std::pair<float, float> &centerLatLon, float minorAxisK);

    inline void removeVortex(TwoDArray &vorField, std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex);
    
    
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


