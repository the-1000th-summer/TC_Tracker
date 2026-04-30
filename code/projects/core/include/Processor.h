#pragma once

#include <atomic>
#include <netcdf>
#include <string>
#include <utility>
#include "Typhoon.h"
#include "Utils.h"
#include <unordered_set>
#include <vector>
#include "multiArray.h"
#include "TC1Time.h"

namespace TTCore {

using CppCallBack = void(__stdcall*)(int stepIdx, void*);
using CppCallBack2 = void(__stdcall*)(double progressValue, void*);

class Processor {
public:
//    Processor(bool* isCanceled, netCDF::NcFile &iFile, bool isWrfoutFile, const std::string &timeVName="", const std::string & latVName="", const std::string & lonVName="", const std::string & vorVName="", int zLevelIndex = -1, const std::string & dumpDirectory = "");
    Processor(std::atomic_bool* shouldCancel, const std::string &iFilePath, bool isWrfoutFile, const VarNames &varNames, int zLevelIndex, double toGridRes, int threadNum, const std::string & dumpDirectory, const std::string &resourceBaseDir);
    //Processor(netCDF::NcFile &iFile, bool isWrfoutFile, const std::string& dumpDirectory);
    ~Processor();
    TCInfo getTCInfo();
    void getDimLength();
    void calcRelativeVorField(netCDF::NcFile *inFile, ThreeDArray& rv);
    void recognizeTyphoon(void(*stepPgCallback)(int stepIdx, void*), void(*progressCallback)(double progressValue, void*), void* target);
    void getRealTC();
    void removeNoise();
//    int getVortexNum1Time(ThreeDArray &vorField, int timeIndex, int TCNum_prevTime);
    int getVortexNum1Time(ThreeDArray &vorField, int timeIndex);
    void getVortexCellsIndex(ThreeDArray &vorField, int timeIndex, std::pair<int, int> maxValIndex, std::unordered_set<std::pair<int, int>, pair_hash> &allCells);
    std::unordered_set<std::pair<int, int>, pair_hash> getSurroundingCellsIndex(std::pair<int, int> cellIndex);
    float get_e(std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex);
    std::pair<float, float> getMinorAxisLen(const std::vector<std::pair<int, int>> &cellsIndex, const std::pair<float, float> &centerLatLon, float minorAxisK, const float gridRatio, const float A);
    
    inline void removeVortex(ThreeDArray &vorField, int timeIndex, std::unordered_set<std::pair<int, int>, pair_hash> vortexCellsIndex);
    
    void dumpStep1(const std::string ncFilePath);
    void dumpStep2(const std::string ncFilePath);
    void dumpStep3(const std::string ncFilePath);
    void getStep1DataFromFile(const std::string &filePath);
    void getStep2DataFromFile(const std::string &filePath);
    void dumpVortexes(const std::vector<std::vector<std::unordered_set<std::pair<int, int>, pair_hash>>> &allVorsCellsIndex);
    void copyRealTCs(std::vector<Typhoon> &tcs);
    void copyTCs(TCs &tcs);
    void copyLatLonData(std::vector<float> &lat_data, std::vector<float> &lon_data);
    
private:
    std::atomic_bool* shouldCancel;
//    netCDF::NcFile *iiFile;
    std::string iFilePath;
    std::unique_ptr<netCDF::NcFile> iiFile;
    VarNames varNames;
    int zLevelIndex = -1;
    std::string dumpDir;
    unsigned long timeLength = 0;
    unsigned long latGridNum = 0;
    unsigned long lonGridNum = 0;
    bool isWrfoutFile = false;
    bool wrfChangeToRegular = false;
    int threadNum = 1;
    std::string resourceBaseDir;
    double toGridRes;
    
    std::unique_ptr<float[]> latArr, lonArr;
    TwoDArray latArr2D = TwoDArray(0,0), lonArr2D = TwoDArray(0,0);
    TCInfo tcInfo;
    std::vector<std::vector<TC1Time>> allVortexes{};
    std::vector<std::vector<std::unordered_set<std::pair<int, int>, pair_hash>>> allVorsCellsIndex;
//    Vortexes vortexes;
    /// 识别出来的纯Typhoon数据
    std::vector<Typhoon> realTCs{};
//    TCs tcs;
    std::vector<std::vector<std::pair<float, float>>> landPolygons;
    std::vector<std::pair<int, int>> set2Vector(const std::unordered_set<std::pair<int, int>, pair_hash> &vortexCellsIndex);
    bool shouldRegrid(float spatialRes);
    std::vector<float> getRgedLatArr(float spatialRes);
    std::vector<float> getRgedLonArr(float spatialRes);
    void regridTheVarData(const std::vector<float> &ref_latData, const std::vector<float> &ref_lonData, const std::string &theVarName, ThreeDArray &theVarField, void(*progressCallback)(double progressValue, void*), void* target);
    void calculateRV(ThreeDArray &uField, ThreeDArray &vField, ThreeDArray &vorField);
    void refreshRgedLatLonData(const std::vector<float> &newLatData, const std::vector<float> &newLonData);
    void unstaggerU(netCDF::NcFile *inFile, ThreeDArray &u_unstged, ThreeDArray &v_unstged);
    
    
    int getLastNotEmptyVecIndex();
    void checkDirAndCreate(const std::string &folderName);
    void getLandPolygons();
    bool pnpoly(const std::vector<std::pair<float, float>>& polygon, float testLat, float testLon);
    bool pnpolys(float testLat, float testLon);

    CppCallBack cppCall;
};

//void to_json(json& j, const TC1Time& r);

}
