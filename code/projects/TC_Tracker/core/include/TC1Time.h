#pragma once

#include <vector>
#include <utility>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include "TCInfo.h"

namespace TTCore {

/// 一个涡旋的数据（位置、index）
class TC1Time {
public:
    //TC1Time(std::pair<int, int> maxVorCellIndex, std::pair<float, float> geoCenter);
    /// 涡旋中心纬度index、经度index
    std::pair<int, int> maxVorCellIndex;
    /// 涡旋中心纬度、经度
    std::pair<float, float> geoCenter;
    /// 涡旋包含的cells的index
    std::vector<std::pair<int, int>> cellsIndex;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & maxVorCellIndex;
        ar & geoCenter;
        ar & cellsIndex;
    }
};

/// 所有时次、每个时次里所有涡旋的信息
class Vortexes {
public:
//    Vortexes(const std::vector<std::vector<TC1Time>> &vortexes, const std::string &timeUnits, double timeInterval) : vortexes(vortexes), tcInfo(timeUnits, timeInterval) {}
    Vortexes(const std::vector<std::vector<TC1Time>> &vortexes, TCInfo &tcInfo) : vortexes(vortexes), tcInfo(tcInfo) {}
    Vortexes() : tcInfo("", 0, 0) {}
    
    
//    void push_back(const )
    inline std::vector<std::vector<TC1Time>> getVortexes() const { return vortexes; }
    inline TCInfo getTcInfo() const { return tcInfo; }
    inline size_t size() const { return vortexes.size(); }
    inline void clearVortexData() { vortexes.clear(); }
    inline std::vector<TC1Time> &operator[](size_t i) { return vortexes[i]; };
private:
    std::vector<std::vector<TC1Time>> vortexes{};
    TCInfo tcInfo;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & vortexes;
        ar & tcInfo;
    }
};

}
