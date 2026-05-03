cmake_minimum_required(VERSION 3.20)
project(TC_Tracker_CLI VERSION 1.0.0)

include(CTest)
enable_testing()

find_package(cxxopts CONFIG REQUIRED)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" "${CMAKE_CURRENT_BINARY_DIR}/core")

add_executable(TC_Tracker_CLI main.cpp)
set_target_properties(TC_Tracker_CLI PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
if(MSVC)
    target_compile_options(TC_Tracker_CLI PRIVATE /utf-8)
endif()
target_link_libraries(TC_Tracker_CLI PRIVATE tc_tracker_core cxxopts::cxxopts)

set(TC_TRACKER_DATA_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../../data/myMap.json")
if(NOT EXISTS "${TC_TRACKER_DATA_FILE}")
    message(FATAL_ERROR "Data file not found: ${TC_TRACKER_DATA_FILE}")
endif()

# print the path to the data file for debugging
message(STATUS "Data file path: ${TC_TRACKER_DATA_FILE}")

# create "data" dir in build dir and copy the data into it
add_custom_command(TARGET TC_Tracker_CLI POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/data")
add_custom_command(TARGET TC_Tracker_CLI POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${TC_TRACKER_DATA_FILE}" "${CMAKE_BINARY_DIR}/data/myMap.json")

# install to -DCMAKE_INSTALL_PREFIX
# (RUNTIME means executable file path)
install(TARGETS TC_Tracker_CLI RUNTIME DESTINATION .)
install(FILES "${TC_TRACKER_DATA_FILE}" DESTINATION data)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
