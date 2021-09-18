#pragma once

#include <utility>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/utility.hpp>

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
    Vortexes(const std::string &timeUnits, double timeInterval) : timeUnits(timeUnits), timeInterval(timeInterval) {}
    std::vector<std::vector<TC1Time>> vortexes{};
    
//    void push_back(const )
    inline size_t size() const;
    inline void clearVortexData();
    inline std::vector<TC1Time> &operator[](size_t i);
private:
    std::string timeUnits;
    double timeInterval;
    
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & vortexes;
        ar & timeUnits;
        ar & timeInterval;
    }
};
inline size_t Vortexes::size() const {
    return vortexes.size();
}
inline void Vortexes::clearVortexData() {
    vortexes.clear();
}
inline std::vector<TC1Time> &Vortexes::operator[](size_t i) {
    return vortexes[i];
}

}
