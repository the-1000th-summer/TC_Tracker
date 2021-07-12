#pragma once

#include <netcdf>
#include <string>
#include "Utils.h"

class Processor {
public:
    Processor(netCDF::NcFile &iFile);
    void recognizeTyphoon();
    void getRealTC();
    void removeNoise();
    void a();
    
    // std::string a("dsf");
private:
    netCDF::NcFile *iiFile;
};
