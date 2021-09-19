//
//  TCInfo.h
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/9/19.
//

#pragma once
#include <iostream>

class TCInfo {
public:
    TCInfo(const std::string &timeUnits, double timeInterval) : timeUnits(timeUnits), timeInterval(timeInterval) {}
    inline std::string getTimeUnits() const { return timeUnits; }
    inline double getTimeInterval() const { return timeInterval; }
private:
    std::string timeUnits;
    double timeInterval;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & timeUnits;
        ar & timeInterval;
    }
};

class VarNames {
public:
    VarNames(const std::string &timeVarName, const std::string &latVarName, const std::string &lonVarName, const std::string &vorVarName) : timeVarName(timeVarName), latVarName(latVarName), lonVarName(lonVarName), vorVarName(vorVarName) {}
    VarNames() {}
    std::string timeVarName;
    std::string latVarName;
    std::string lonVarName;
    std::string vorVarName;
    void checkVarNames() {
        if (timeVarName.empty()) {
            std::cout << "Please enter time variable name:" << std::endl;
            getline(std::cin, timeVarName);
        }
        if (latVarName.empty()) {
            std::cout << "Please enter lat variable name:" << std::endl;
            getline(std::cin, latVarName);
        }
        if (lonVarName.empty()) {
            std::cout << "Please enter lon variable name:" << std::endl;
            getline(std::cin, lonVarName);
        }
        if (vorVarName.empty()) {
            std::cout << "Please enter vorticity variable name:" << std::endl;
            getline(std::cin, vorVarName);
        }
    }
};
