#pragma once

#include <utility>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/utility.hpp>

namespace TTCore {
    class TC1Time {
    public:
        // TC1Time();
        std::pair<int, int> maxVorCellIndex;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version) {
            ar & maxVorCellIndex;
        }
    };
}
