#include <ctime>
#include <iostream>
#include <chrono>
#include <netcdf>
#include <vector>
// #include "Typhoon.h"
#include "Processor.h"
#include "Utils.h"

using namespace std;

void tryReadNCFile() {
    
    // vector<float> latVec, lonVec;
    // UtilFunc::getLatLonData(iFile, latVec, lonVec);
    // UtilFunc::getTimeData(iFile);
    cout << "sdf" << endl;
}

int main(int, char**) {
    netCDF::NcFile iFile("/mnt/e/University/TC_Tracker/data/Vorticity_JRA-55_hourly.nc", netCDF::NcFile::read);
    // Processor p(iFile);
    Processor p(iFile);
    p.a();

    // time_t timep = 0;
    // auto a = gmtime(&timep);
    // cout << asctime(a) << endl;
    // Typhoon a;
    // a.sayHello();
    // tryReadNCFile();
    // UtilFunc::getEpochTime(L"1970-01-01T00:00:00Z");
    
    // auto aa = UtilFunc::getEpochTime("hours since 1800-sd01-01 00:00", "hours since %Y-%m-%d %H:%M");
    // cout << aa << endl;
    // cout << UtilFunc::ifFileExists("sdf") << endl;
    // cout << UtilFunc::ifFileExists("const std::string &name") << endl;

    // cout << asctime(&aa) << endl;
    // cout << ctime(&timep) << endl;
}
