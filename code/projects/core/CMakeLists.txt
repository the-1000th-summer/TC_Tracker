cmake_minimum_required(VERSION 3.20)

project(tc_tracker_core LANGUAGES CXX)

find_package(Boost 1.70 REQUIRED COMPONENTS serialization filesystem)
find_package(netCDF REQUIRED)
find_package(netCDFCxx CONFIG QUIET)
find_package(nlohmann_json CONFIG QUIET)
find_package(OpenMP REQUIRED)
# cmake may not find abseil if only write
# `find_package(absl CONFIG REQUIRED)`
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

if(NOT TARGET netCDF::netcdf-cxx4)
    find_program(NetCDFCxx_CONFIG_EXECUTABLE NAMES ncxx4-config)

    if(NetCDFCxx_CONFIG_EXECUTABLE)
        execute_process(
            COMMAND "${NetCDFCxx_CONFIG_EXECUTABLE}" --includedir
            OUTPUT_VARIABLE NetCDFCxx_CONFIG_INCLUDE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
            COMMAND "${NetCDFCxx_CONFIG_EXECUTABLE}" --libdir
            OUTPUT_VARIABLE NetCDFCxx_CONFIG_LIBRARY_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()

    find_path(NetCDFCxx_INCLUDE_DIR
        NAMES netcdf
        HINTS "${NetCDFCxx_CONFIG_INCLUDE_DIR}"
    )
    find_library(NetCDFCxx_LIBRARY
        NAMES netcdf-cxx4 netcdf_c++4
        HINTS "${NetCDFCxx_CONFIG_LIBRARY_DIR}"
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(netCDFCxx
        REQUIRED_VARS NetCDFCxx_LIBRARY NetCDFCxx_INCLUDE_DIR
    )

    add_library(netCDF::netcdf-cxx4 UNKNOWN IMPORTED)
    set_target_properties(netCDF::netcdf-cxx4 PROPERTIES
        IMPORTED_LOCATION "${NetCDFCxx_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${NetCDFCxx_INCLUDE_DIR}"
    )

    if(TARGET netCDF::netcdf)
        set_target_properties(netCDF::netcdf-cxx4 PROPERTIES
            INTERFACE_LINK_LIBRARIES netCDF::netcdf
        )
    elseif(netCDF_LIBRARIES)
        set_target_properties(netCDF::netcdf-cxx4 PROPERTIES
            INTERFACE_LINK_LIBRARIES "${netCDF_LIBRARIES}"
        )
    endif()
endif()

set(NCL_FUNC_CXX_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/NCL_func_cxx")

# Generate C++ source files from the TCsP.proto file
protobuf_generate_cpp(TCS_PROTO_SRCS TCS_PROTO_HDRS TCsP.proto)

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


# Include directories
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


# Link libraries
target_link_libraries(tc_tracker_core
    PUBLIC
        Boost::serialization
        Boost::filesystem
        netCDF::netcdf-cxx4
)

if(TARGET nlohmann_json::nlohmann_json)
    target_compile_definitions(tc_tracker_core PRIVATE TTCORE_USE_SYSTEM_NLOHMANN_JSON)
    target_link_libraries(tc_tracker_core PRIVATE nlohmann_json::nlohmann_json)
endif()

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
