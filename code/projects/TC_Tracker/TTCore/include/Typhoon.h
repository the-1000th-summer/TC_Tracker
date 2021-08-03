#pragma once

#include <utility>
#include <vector>

namespace TTCore {
    class Typhoon {
    public:
        // Typhoon();
        int serialNo;
        std::vector<std::pair<int, int>> maxVorCells;
        int startTimeIndex;
        int endTimeIndex;
    private:

    };
}

