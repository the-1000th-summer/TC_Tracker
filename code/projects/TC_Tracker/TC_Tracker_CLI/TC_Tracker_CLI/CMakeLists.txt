cmake_minimum_required(VERSION 3.0.0)
project(TC_Tracker_CLI VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
# project include file and source file
include_directories(
    ../../core/include
)
aux_source_directory(../../core/src SRC_LIST)

# find boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70 REQUIRED COMPONENTS serialization filesystem ) 
find_package(netCDF REQUIRED)
find_package(OpenMP REQUIRED)

if(Boost_FOUND AND netCDF_FOUND AND OPENMP_FOUND)
    # set for OpenMP
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    
    find_library(libncxxPath netcdf-cxx4 ${netCDF_LIB_DIR})
    add_executable(TC_Tracker_CLI main.cpp ${SRC_LIST})
    # create "data" dir in build dir and copy the data into it
    add_custom_command(TARGET TC_Tracker_CLI POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/data")
    add_custom_command(TARGET TC_Tracker_CLI POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/../../../../../data/myMap/myMap.dat" "${CMAKE_BINARY_DIR}/data")
    target_link_libraries(TC_Tracker_CLI Boost::serialization Boost::filesystem OpenMP::OpenMP_CXX ${libncxxPath})
    # install to -DCMAKE_INSTALL_PREFIX
    # (RUNTIME means executable file path)
    install (TARGETS TC_Tracker_CLI RUNTIME DESTINATION .)
    install(FILES build/data/myMap.dat DESTINATION data)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
