#pragma once

#include <netcdf>
#include <string>
#include <utility>
#include "Typhoon.h"
#include "Utils.h"
#include <unordered_set>
#include <vector>
#include "multiArray.h"
#include "TC1Time.h"
#include "json.hpp"

namespace TTCore {

    class Processor {
    public:
        Processor(netCDF::NcFile &iFile, const std::string &latVName, const std::string &lonVName, const std::string &varVName, const std::string &dumpDirectory);
        ~Processor();
        void getDimLength();
        void recognizeTyphoon();
        void getRealTC();
        void removeNoise();
        int getVortexNum1Time(ThreeDArray &vorField, int timeIndex, int TCNum_prevTime);
        void getVortexCellsIndex(ThreeDArray &vorField, int timeIndex, std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCells);
        std::unordered_set<std::pair<int, int>, pair_hash> getSurroundingCellsIndex(std::pair<int, int> cellIndex);
        float get_e(std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex);
        std::pair<float, float> getMinorAxisLen(const std::vector<std::pair<int, int>> &cellsIndex, const std::pair<float, float> &centerLatLon, float minorAxisK, const float gridRatio, const float A);

        inline void removeVortex(ThreeDArray &vorField, int timeIndex, std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex);

        void dumpStep1();
        void dumpStep2();
        void dumpStep3();
        void getStep1DataFromFile(const std::string &filePath);
        void getStep2DataFromFile(const std::string &filePath);
        void copyRealTCs(std::vector<Typhoon> &tcs);
    
    private:
        netCDF::NcFile *iiFile;
        std::string latVarName;
        std::string lonVarName;
        std::string vorVarName;
        std::string dumpDir;
        unsigned long timeLength = 0;
        unsigned long latGridNum = 0;
        unsigned long lonGridNum = 0;
        /// 记录所有有台风的日期的数组（从基准日期起算的时次数）
        std::unique_ptr<float[]> latArr, lonArr;
        std::vector<int> hasTC_timeIndex{};
        std::vector<std::vector<TC1Time>> allVortexes{};
        std::vector<Typhoon> realTCs{};
        std::vector<std::vector<std::pair<float, float>>> landPolygons;

        void checkDirAndCreate(const std::string &folderName);
        void getLandPolygons();
        bool pnpoly(const std::vector<std::pair<float, float>>& polygon, float testLat, float testLon);
        bool pnpolys(float testLat, float testLon);
    };

    //void to_json(json& j, const TC1Time& r);

}
