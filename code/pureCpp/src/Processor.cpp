
#include "Processor.h"
#include "Utils.h"
#include <iostream>
#include <iterator>
#include <netcdf>
#include <ostream>
#include <vector>

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

void Processor::recognizeTyphoon() {
    std::cout << "第一步(recognize_typhoon): 导入文件成功，开始识别。" << std::endl;

    std::vector<int> hasTp_timeIndex{};

    int tpNum_prevDay = 0;
}



