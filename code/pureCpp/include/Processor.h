#pragma once

#include <netcdf>
#include <string>
#include <utility>
#include "Utils.h"

class Processor {
public:
    Processor(netCDF::NcFile &iFile);
    void recognizeTyphoon(float vorField[Constants::latGridNum][Constants::lonGridNum]);
    void getRealTC();
    void removeNoise();
    void a();
    void getVortexNum1Day(float vorField[Constants::latGridNum][Constants::lonGridNum]);
    void getVortexCells(float vorField[Constants::latGridNum][Constants::lonGridNum], std::pair<int, int> maxValIndex, std::vector<std::pair<int, int>> &allCells);
    void getOuterCells();
    
    // std::string a("dsf");
private:
    netCDF::NcFile *iiFile;
};
