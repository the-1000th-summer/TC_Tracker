cmake_minimum_required(VERSION 3.5.0)
project(TC_Tracker_CLI VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" "${CMAKE_CURRENT_BINARY_DIR}/core")

add_executable(TC_Tracker_CLI main.cpp)
set_target_properties(TC_Tracker_CLI PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
if(MSVC)
    target_compile_options(TC_Tracker_CLI PRIVATE /utf-8)
endif()
target_link_libraries(TC_Tracker_CLI PRIVATE tc_tracker_core)

# print the path to the data file for debugging
message(STATUS "Data file path: ${PROJECT_SOURCE_DIR}/../../../data/myMap/myMap.dat")

# create "data" dir in build dir and copy the data into it
add_custom_command(TARGET TC_Tracker_CLI POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/data")
add_custom_command(TARGET TC_Tracker_CLI POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../../../data/myMap/myMap.dat" "${CMAKE_BINARY_DIR}/data")

# install to -DCMAKE_INSTALL_PREFIX
# (RUNTIME means executable file path)
install(TARGETS TC_Tracker_CLI RUNTIME DESTINATION .)
install(FILES "${CMAKE_BINARY_DIR}/data/myMap.dat" DESTINATION data)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
