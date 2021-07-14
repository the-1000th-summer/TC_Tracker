#pragma once

#include <netcdf>
#include <string>
#include <utility>
#include "Utils.h"
#include <unordered_set>
#include <vector>
#include "TC1Time.h"


class Processor {
public:
    Processor(netCDF::NcFile &iFile);
    void recognizeTyphoon(float vorField[Constants::latGridNum][Constants::lonGridNum]);
    void getRealTC();
    void removeNoise();
    int getVortexNum1Time(float vorField[Constants::latGridNum][Constants::lonGridNum], int tpNum_prevTime);
    void getVortexCellsIndex(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCells);
    std::unordered_set<std::pair<int, int>, pair_hash> getSurroundingCellsIndex(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> cellIndex);
    void get_e();
    void removeVortex(float vorField[Constants::latGridNum][Constants::lonGridNum], std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex);
    
private:
    netCDF::NcFile *iiFile;
    /// 记录所有有台风的日期的数组（从基准日期起算的时次数）
    float lonArr[Constants::lonGridNum], latArr[Constants::latGridNum];
    std::vector<int> hasTC_timeIndex{};
    std::vector<std::vector<TC1Time>> allVortexes{};
};
