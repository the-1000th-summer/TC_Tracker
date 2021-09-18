#pragma once

#include <utility>
#include <vector>
#include <utility>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace TTCore {

class Typhoon {
public:
    // Typhoon();
    int serialNo;
    std::vector<std::pair<int, int>> maxVorCells;
    std::vector<std::pair<float, float>> geoCenters;
    int startTimeIndex;
    int endTimeIndex;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & serialNo;
        ar & maxVorCells;
        ar & geoCenters;
        ar & startTimeIndex;
        ar & endTimeIndex;
    }
};

class TCs {
public:
    TCs(const std::vector<Typhoon> &tcs, const std::string &timeUnits, double timeInterval) : tcs(tcs), timeUnits(timeUnits), timeInterval(timeInterval) {
    }
    std::vector<Typhoon> tcs;
    
    inline std::string getTimeUnits() const;
    inline double getTimeInterval() const;
    inline size_t size() const;
    inline std::vector<Typhoon>::const_iterator cbegin() const;
    inline std::vector<Typhoon>::const_iterator cend() const;
private:
    std::string timeUnits;
    double timeInterval;
};
// return by value! 编译器会进行优化以避免不必要的数据复制
// https://stackoverflow.com/questions/10553091/what-is-the-best-way-to-return-string-in-c
inline std::string TCs::getTimeUnits() const {
    return timeUnits;
}
inline double TCs::getTimeInterval() const {
    return timeInterval;
}
inline size_t TCs::size() const {
    return tcs.size();
}
inline std::vector<Typhoon>::const_iterator TCs::cbegin() const {
    return tcs.cbegin();
}
inline std::vector<Typhoon>::const_iterator TCs::cend() const {
    return tcs.cend();
}

}
