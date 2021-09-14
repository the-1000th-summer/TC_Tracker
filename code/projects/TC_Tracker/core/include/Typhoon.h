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
}

