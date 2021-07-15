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


void tryMKTIME() {
    // tm dt{0, 0, 0, };

}

void vv(float a[1000][Constants::latGridNum][Constants::lonGridNum]) {

}

int main(int, char**) {
    
    // netCDF::NcFile iFile("/mnt/e/University/TC_Tracker/data/Vorticity_JRA-55_hourly_timeIndex19.nc", netCDF::NcFile::read);
    // float aaa[Constants::latGridNum][Constants::lonGridNum];
    // auto vorVar = iFile.getVar("Vorticity");
    // vorVar.getVar(aaa);
    // iFile.close();


    // Processor p(iFile);
    // p.recognizeTyphoon(aaa);

    std::vector<int> a{ 1,4, 65, 6};
    vector<int> b{3};
    b = a;
    for (const auto &i : b) {
        cout << i << endl;
    }
    

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
