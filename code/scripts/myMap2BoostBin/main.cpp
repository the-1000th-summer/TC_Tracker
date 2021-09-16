#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include "json.hpp"

//using namespace n

void dumpNoIsland() {
    std::string myMapFileDir = "E:\\University\\TC_Tracker\\data\\myMap\\";
    std::string myMapFilePath = myMapFileDir + "myMap_noIsland.json";

    std::ifstream i(myMapFilePath);
    nlohmann::json j;
    i >> j;

    /// polygon
    auto mainLand = j["features"][0]["geometry"]["coordinates"][0];
    std::vector<std::vector<std::pair<float,float>>> mL_vector_output;
    std::vector<std::pair<float, float>> mL_vector;
    for (const auto& point : mainLand) {
        mL_vector.push_back({point[1], point[0]});
        //std::cout << ii << std::endl;
    }
    mL_vector_output.push_back(mL_vector);

    std::ofstream ofs(myMapFileDir + "myMap_noIsland.dat", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    // write class instance to archive
    oa << mL_vector_output;

    std::cout << "finished!" << std::endl;
}

void dumpMyMap() {
    std::string myMapFileDir = "/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/myMap/";
    std::string myMapFilePath = myMapFileDir + "myMap.json";

    std::ifstream i(myMapFilePath);
    nlohmann::json j;
    i >> j;

    std::vector<std::vector<std::pair<float, float>>> mL_vector_output;
    

    auto polygons = j["features"];
    for (auto& polygon : polygons) {
        auto points = polygon["geometry"]["coordinates"][0];
        std::vector<std::pair<float, float>> mL_vector;
        for (const auto& point : points) {
            mL_vector.push_back({ point[1], point[0] });
        }
        mL_vector_output.push_back(mL_vector);
    }

    std::ofstream ofs(myMapFileDir + "myMap.dat", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    // write class instance to archive
    oa << mL_vector_output;

    std::cout << "finished!" << std::endl;
}

void testMyMap_noIsland() {
    std::string myMapFileDir = "E:\\University\\TC_Tracker\\data\\myMap\\";
    std::string myMapFilePath = myMapFileDir + "myMap_noIsland.dat";

    std::ifstream ifs(myMapFilePath, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);

    std::vector<std::vector<std::pair<float, float>>> mL_vector;
    ia >> mL_vector;
    auto points = mL_vector[0];
    for (const auto& point : points) {
        std::cout << point.first << ", " << point.second << std::endl;
    }
}

void testMyMap() {
    std::string myMapFileDir = "E:\\University\\TC_Tracker\\data\\myMap\\";
    std::string myMapFilePath = myMapFileDir + "myMap.dat";

    std::ifstream ifs(myMapFilePath, std::ios::binary);
    boost::archive::binary_iarchive ia(ifs);

    std::vector<std::vector<std::pair<float, float>>> mL_vector;
    ia >> mL_vector;

    for (const auto& polygon : mL_vector) {
        for (const auto& point : polygon) {
            std::cout << point.first << ", " << point.second << std::endl;
        }
        std::cout << "--------\n\n" << std::endl;
    }
    
}

int main() {
    //dumpNoIsland();
    dumpMyMap();
    //testMyMap_noIsland();
    // testMyMap();

}
