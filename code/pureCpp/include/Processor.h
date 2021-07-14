#pragma once

#include <netcdf>
#include <string>
#include <utility>
#include "Utils.h"
#include <unordered_set>

class Processor {
public:
    Processor(netCDF::NcFile &iFile);
    void recognizeTyphoon(float vorField[Constants::latGridNum][Constants::lonGridNum]);
    void getRealTC();
    void removeNoise();
    void a();
    void getVortexNum1Day(float vorField[Constants::latGridNum][Constants::lonGridNum]);
    void getVortexCells(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCells);
    std::unordered_set<std::pair<int, int>, pair_hash> getSurroundingCells(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> cellIndex);
    
    // std::string a("dsf");
private:
    netCDF::NcFile *iiFile;
};
