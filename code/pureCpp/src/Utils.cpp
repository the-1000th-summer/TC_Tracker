#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Utils.h"
#include <netcdf>
#include <sys/stat.h>
#include <vector>



bool UtilFunc::ifFileExists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

    
/// 将UTC时间字符串转换成UNIX时间戳（1970年1月1日00:00开始所经过的秒数）
/// @param[in] dataTime UTC时间字符串
/// @return UNIX时间戳
std::time_t UtilFunc::getEpochTime(const std::string& dateTime, const std::string& dateTimeFormat){
    // Let's consider we are getting all the input in
    // this format: '2014-07-25T20:17:22Z' (T denotes
    // start of Time part, Z denotes UTC zone).
    // A better approach would be to pass in the format as well.
    // static const std::wstring dateTimeFormat{ L"%Y-%m-%dT%H:%M:%SZ" };

    // Create a stream which we will use to parse the string,
    // which we provide to constructor of stream to fill the buffer.
    std::stringstream ss{ dateTime };
    std::tm dt{};
    // Now we read from buffer using get_time manipulator
    // and formatting the input appropriately.
    ss >> std::get_time(&dt, dateTimeFormat.c_str());
    if (ss.fail()) {
        throw std::runtime_error("parse time str failed.");
    }

    // mktime() interprets its input as local time
    // Minus timezone to get UTC time
    return std::mktime(&dt) - timezone;
}

void UtilFunc::getTimeData(netCDF::NcFile &iFile) {
    auto timeVar = iFile.getVar("time");
    std::string timeUnits;
    timeVar.getAtt("units").getValues(timeUnits);
    std::cout << timeUnits << std::endl;
}

void UtilFunc::getLatLonData(netCDF::NcFile *iFile, std::vector<float> &latVec, std::vector<float> &lonVec) {
    float lat[41], lon[89];
    iFile->getVar("lat").getVar(lat);
    iFile->getVar("lon").getVar(lon);
    latVec.assign(lat, lat + sizeof(lat) / sizeof(lat[0]) );
    lonVec.assign(lon, lon + sizeof(lon) / sizeof(lon[0]) );
}

/// 此函数将相对涡度从文件中提取出来
void UtilFunc::getVorField(netCDF::NcFile *iFile, float *vor) {
    iFile->getVar("Vorticity").getVar(vor);
}
