cmake_minimum_required(VERSION 3.5.0)

project(tc_tracker_core LANGUAGES CXX)

find_package(Boost 1.70 REQUIRED COMPONENTS serialization filesystem)
find_package(netCDF REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Protobuf REQUIRED)

set(NCL_FUNC_CXX_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/NCL_func_cxx")

# Generate C++ source files from the TCsP.proto file
protobuf_generate_cpp(TCS_PROTO_SRCS TCS_PROTO_HDRS TCsP.proto)

find_library(TC_TRACKER_NETCDF_CXX_LIBRARY
    NAMES netcdf-cxx4 netcdf_c++4
    HINTS
        ${netCDF_LIB_DIR}
        ${netCDF_LIBRARY_DIRS}
)

if(NOT TC_TRACKER_NETCDF_CXX_LIBRARY)
    message(FATAL_ERROR "Could not find the netCDF C++4 library.")
endif()

add_library(tc_tracker_core STATIC
    src/NCFileInfo.cpp
    src/Processor.cpp
    src/TC1Time.cpp
    src/Typhoon.cpp
    src/Utils.cpp
    src/uv2vr_cfd.cpp
    src/VortexesDumper.cpp
    ${TCS_PROTO_SRCS}
    ${TCS_PROTO_HDRS}
    "${NCL_FUNC_CXX_DIR}/src/dmapgci.cpp"
    "${NCL_FUNC_CXX_DIR}/src/linint2.cpp"
    "${NCL_FUNC_CXX_DIR}/src/linmsg_dp.cpp"
    "${NCL_FUNC_CXX_DIR}/src/rcm2rgrid.cpp"
)

set_target_properties(tc_tracker_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if(MSVC)
    target_compile_options(tc_tracker_core PRIVATE /utf-8)
endif()

target_include_directories(tc_tracker_core
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${NCL_FUNC_CXX_DIR}/include"
        ${Protobuf_INCLUDE_DIRS}
)

if(netCDF_INCLUDE_DIRS)
    target_include_directories(tc_tracker_core PUBLIC ${netCDF_INCLUDE_DIRS})
elseif(netCDF_INCLUDE_DIR)
    target_include_directories(tc_tracker_core PUBLIC ${netCDF_INCLUDE_DIR})
endif()

target_link_libraries(tc_tracker_core
    PUBLIC
        Boost::serialization
        Boost::filesystem
        ${TC_TRACKER_NETCDF_CXX_LIBRARY}
)

if(TARGET netCDF::netcdf)
    target_link_libraries(tc_tracker_core PUBLIC netCDF::netcdf)
elseif(netCDF_LIBRARIES)
    target_link_libraries(tc_tracker_core PUBLIC ${netCDF_LIBRARIES})
endif()

if(TARGET OpenMP::OpenMP_CXX)
    target_link_libraries(tc_tracker_core PUBLIC OpenMP::OpenMP_CXX)
else()
    target_compile_options(tc_tracker_core PUBLIC ${OpenMP_CXX_FLAGS})
    target_link_libraries(tc_tracker_core PUBLIC ${OpenMP_CXX_FLAGS})
endif()

if(TARGET protobuf::libprotobuf)
    target_link_libraries(tc_tracker_core PRIVATE protobuf::libprotobuf)
else()
    target_link_libraries(tc_tracker_core PRIVATE ${Protobuf_LIBRARIES})
endif()
