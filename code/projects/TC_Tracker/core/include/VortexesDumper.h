//
//  VortexesDumper.h
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/10/15.
//

#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_set>
//#include "Utils.h"
#include "TCInfo.h"

namespace TTCore {

class VortexesDumper {
public:
    VortexesDumper(const std::string vorNcFilePath, const std::string &oFilePath, const TCInfo &tcInfo);
    void setLatLonData(const float* const latArr, size_t latDimLen, const float* const lonArr, size_t lonDimLen);
    void dumpVortexes2NC(const std::vector<std::vector<std::unordered_set<std::pair<int, int>, pair_hash>>> &allVorsCellsIndex);
#ifdef GETVORBASEDONREAL
    void dumpVortexes2Proto3(const std::vector<std::vector<std::unordered_set<std::pair<int, int>, TTCore::pair_hash>>> &allVorsCellsIndex, const std::vector<std::vector<int>> &findRank);
#endif
private:
    std::string vorNcFilePath;
    std::string oFilePath;
    TCInfo tcInfo;
    size_t latDimLen = 0, lonDimLen = 0;
    std::unique_ptr<float[]> timeData, latData, lonData;
    bool latLonDataDone = false;
    
    void fillTimeData(float *timeData, size_t dataLen);
    void fillVorData(const std::vector<std::vector<std::unordered_set<std::pair<int, int>, pair_hash>>> &allVorsCellsIndex, ThreeDArray &vorData);
    void copyAllCharAtt(const netCDF::NcVar &inVar, netCDF::NcVar &oVar);
};

}
