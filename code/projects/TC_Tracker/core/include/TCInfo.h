//
//  TCInfo.h
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/9/19.
//

#pragma once
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <iostream>

/// 存储台风的总体信息
class TCInfo {
public:
    TCInfo(const std::string &timeUnits, double timeInterval, double firstTValue) : timeUnits(timeUnits), timeInterval(timeInterval), firstTValue(firstTValue) {}
    inline std::string getTimeUnits() const { return timeUnits; }
    inline double getTimeInterval() const { return timeInterval; }
    inline double getFirstTValue() const { return firstTValue; }
    double getHourInterval() const {
        double timeInterval = getTimeInterval();
        std::string timeUnitLen = timeUnits.substr(0, timeUnits.find(" "));
        if (timeUnitLen == "minutes") {
            return timeInterval / 60.0;
        } else if (timeUnitLen == "hours") {
            return timeInterval;
        } else if (timeUnitLen == "days") {
            return timeInterval * 24.0;
        } else if (timeUnitLen == "seconds") {
            return timeInterval / 3600.0;
        } else {
            throw std::runtime_error("未知单位: " + timeUnitLen);
        }
    }
private:
    std::string timeUnits;
    double timeInterval;
    /// 文件中的time variable第一个数据的值
    double firstTValue;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & timeUnits;
        ar & timeInterval;
        ar & firstTValue;
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
