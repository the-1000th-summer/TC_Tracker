#pragma once

#include <utility>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include "TCInfo.h"

namespace TTCore {
class TC1Time {
public:
    //TC1Time(std::pair<int, int> maxVorCellIndex, std::pair<float, float> geoCenter);
    std::pair<int, int> maxVorCellIndex;
    std::pair<float, float> geoCenter;
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & maxVorCellIndex;
        ar & geoCenter;
    }
};

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
