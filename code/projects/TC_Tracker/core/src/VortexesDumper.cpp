//
//  VortexesDumper.cpp
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/10/15.
//


#include <netcdf>
#include "VortexesDumper.h"


void VortexesDumper::dumpVortexes2NC(std::vector<std::vector<std::unordered_set<std::pair<int, int>, TTCore::pair_hash>>> &allVorsCellsIndex, const std::string &oFilePath) {
    netCDF::NcFile oFile(oFilePath, netCDF::NcFile::replace);
    
    
    oFile.close();
}
