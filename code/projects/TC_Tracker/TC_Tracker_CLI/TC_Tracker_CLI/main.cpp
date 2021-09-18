//
//  main.cpp
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/9/14.
//

#include <iostream>
#include <netcdf>
#include <boost/program_options.hpp>
#include "cxxopts.hpp"
#include "NCFileInfo.h"

namespace bpo = boost::program_options;

//inline void handleHelpAndVersion(const cxxopts::ParseResult &result) {
//    if (result.count("help")) {
//        std::cout << options.help({""}) << std::endl;
//        exit(0);
//    }
//    if (result.count("version")) {
//        std::cout << "TC_Tracker_CLI v0.1" << std::endl;
//        exit(0);
//    }
//}

inline void abortWithMsg(const std::string &msg) {
    std::cout << msg << std::endl;
    std::cout << "Aborted." << std::endl;
    exit(1);
}

/// 处理输入输出的文件名，并打印文件名信息
/// vector的最后一个元素视为输出文件，其他元素视为输入文件
inline void handleInOutFile(const std::vector<std::string> &fileNames) {
    if (fileNames.size() == 1) {
        abortWithMsg("Please specify output file name!");
    }
    std::cout << "TC_Tracker start." << std::endl;
    std::cout << "Input file(s):" << std::endl;
    for (int i = 0; i < fileNames.size()-1; ++i) {
        std::cout << "  " << fileNames[i] << std::endl;
    }
    std::cout << "Output file:\n  " << fileNames.back() << std::endl;
}



void tryCXXOPTS(int argc, char * argv[]) {
    cxxopts::Options options("TC_Tracker", "TC_Tracker: Tropical Cyclone tracking program");
    options.add_options()
    ("h,help", "Display available options") // a bool parameter
    ("v,version", "Print version information")
    ("t,no-temp-files", "Do not export temp files", cxxopts::value<bool>()->default_value("true"))
    ;
    // hidden options
    options.add_options("hiddenOpts")
    ("inOutFiles", "Input and output file names", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"inOutFiles"});
    options.positional_help("input.nc output.nc");
    // 解析
    auto result = options.parse(argc, argv);
//    handleHelpAndVersion(result);
    if (result.count("help")) {
        std::cout << options.help({""}) << std::endl;
        exit(0);
    }
    if (result.count("version")) {
        std::cout << "TC_Tracker_CLI v0.1" << std::endl;
        exit(0);
    }
    // 处理输入输出文件
    std::vector<std::string> allFilesName{};
    try {
        allFilesName = result["inOutFiles"].as<std::vector<std::string>>();
    } catch (std::domain_error) {         // 没有positional arguments
        abortWithMsg("Please specify input and output files!");
    }
    handleInOutFile(allFilesName);
    
    /// （无用的变量，因为cli直接Ctrl+C就可终止程序）
    bool isCanceled = false;
    std::string ncFileDir = "/Users/richard/Documents/p_learn/cpp_learn/TC_Tracker/data/";
    auto fileInfo = TTCore::NCFileInfo((ncFileDir+"wrfout_d01_2016-10-19_00_00_00_persist_minForVor.nc").c_str(), true, "XTIME", "XLAT", "XLONG", "", 0, ".");
    
    std::vector<TTCore::Typhoon> tcs{};
    fileInfo.startTracking(tcs, &isCanceled);
//    fileInfo.exportFile_nc(tcs, ncFileDir+"minForVor_track.nc");
    fileInfo.exportFile_nc_compact(tcs, ncFileDir+"minForVor_track_compact.nc");
    std::cout << "end of tracking" << std::endl;
}

void tryBoostOptions(int argc, char * argv[]) {
    bpo::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Display available options")
        ("version,v", "Print version information")
        ("compression", bpo::value<int>(), "set compression level");
    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return;
    }
    if (vm.count("version")) {
        std::cout << "TC_Tracker_CLI v0.1" << std::endl;
        return;
    }

    if (vm.count("compression")) {
        std::cout << "Compression level was set to "
     << vm["compression"].as<int>() << ".\n";
    } else {
        std::cout << "Compression level was not set.\n";
    }

}


int main(int argc, char * argv[]) {
    
//    netCDF::NcFile("dsf", netCDF::NcFile::read);
    tryCXXOPTS(argc, argv);
//    tryBoostOptions(argc, argv);
    std::cout << "done!" << std::endl;
}
