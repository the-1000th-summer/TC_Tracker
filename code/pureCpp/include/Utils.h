#pragma once

#include <ctime>
#include <string>
#include <netcdf>
#include <vector>

namespace Constants {
    constexpr int TODAY_MAX_TP_NUM = 5;
    constexpr float HAS_TP_MIN_ReVOR = 2e-5;
    constexpr int TP_MIN_PTS = 5;
    constexpr float TP_MIN_E = 0.9;
    constexpr float RECURSION_MIN_ReVOR = 4e-5;
    constexpr float LINK_TP_MAX_DIST = 300.0;

    const std::string VOR_FILE_DIR = "/mnt/e/University/TC_Tracker/data/";
    const std::string VOR_FILE_PATH = VOR_FILE_DIR + "Vorticity_JRA-55_hourly.nc";
}

namespace UtilFunc {
    bool ifFileExists (const std::string& name);
    std::time_t getEpochTime(const std::string& dateTime, const std::string& dateTimeFormat="%Y-%m-%dT%H:%M:%SZ");
    void getTimeData(netCDF::NcFile &iFile);
    void getLatLonData(netCDF::NcFile *iFile, std::vector<float> &latVec, std::vector<float> &lonVec);
    void getVorField(netCDF::NcFile *iFile, float *vor);
}
