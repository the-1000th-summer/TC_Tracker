#include <cstdio>
#include <ctime>
#include <iostream>
#include <chrono>
#include <netcdf>
#include <vector>
// #include "Typhoon.h"
#include "Processor.h"
#include "Utils.h"
#include <algorithm>

using namespace std;



int main(int, char**) {
    
    netCDF::NcFile iFile("/mnt/e/University/TC_Tracker/data/Vorticity_JRA-55_hourly.nc", netCDF::NcFile::read);
    
    
    Processor p(iFile);
    p.recognizeTyphoon();
    
    
    // time_t timep = 0;
    // auto a = gmtime(&timep);
    // cout << asctime(a) << endl;
    // Typhoon a;
    // a.sayHello();
    // tryReadNCFile();
    // UtilFunc::num2Date(45, "hours since 1800-01-01 00:00");
    // UtilFunc::getEpochTime(L"1970-01-01T00:00:00Z");
    // tryMKTIME();
    // auto aa = UtilFunc::getEpochTime("hours since 1800-01-01 00:00", "hours since %Y-%m-%d %H:%M");

    // std::tm dt{};
    // UtilFunc::num2Date(0, dt, "hours since 0000-01-01 00:00");
    // cout << asctime(&dt) << endl;

    // cout << aa << endl;
    // cout << UtilFunc::ifFileExists("sdf") << endl;
    // cout << UtilFunc::ifFileExists("const std::string &name") << endl;

    // cout << asctime(&aa) << endl;
    // cout << ctime(&timep) << endl;
}
