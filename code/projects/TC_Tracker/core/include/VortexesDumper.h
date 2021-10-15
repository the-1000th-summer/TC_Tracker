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
#include "Utils.h"

class VortexesDumper {
public:
    VortexesDumper();
    void dumpVortexes2NC(std::vector<std::vector<std::unordered_set<std::pair<int, int>, TTCore::pair_hash>>> &allVorsCellsIndex, const std::string &oFilePath);
private:
    std::string oFilePath;
};
