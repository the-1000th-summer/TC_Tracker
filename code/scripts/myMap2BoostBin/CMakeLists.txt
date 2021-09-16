cmake_minimum_required(VERSION 3.0.0)
project(myMap2BoostBin VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

set(boostDir /opt/homebrew/Cellar/boost/1.76.0)

include_directories(
    ${boostDir}/include
)

add_executable(myMap2BoostBin main.cpp)

find_library(libboostPath boost_serialization ${boostDir}/lib)

target_link_libraries(myMap2BoostBin ${libboostPath})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
