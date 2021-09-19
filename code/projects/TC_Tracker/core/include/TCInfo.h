//
//  TCInfo.h
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/9/19.
//

#pragma once

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
