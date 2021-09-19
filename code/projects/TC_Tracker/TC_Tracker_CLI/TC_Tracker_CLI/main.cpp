//
//  main.cpp
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/9/14.
//

#include <iostream>
#include <netcdf>
#include <filesystem>
#include <boost/program_options.hpp>
#include "cxxopts.hpp"
#include "NCFileInfo.h"
#include "Typhoon.h"

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
    auto exePath = std::filesystem::weakly_canonical(std::filesystem::path(argv[0])).parent_path();

    cxxopts::Options options("TC_Tracker", "TC_Tracker: Tropical Cyclone tracking program");
    options.add_options()
    ("h,help", "Display available options") // a bool parameter
    ("v,version", "Print version information")
    ("z,z-lv-index", "Specify the index of z level", cxxopts::value<int>())
    ("t,no-temp-files", "Do not export temp files", cxxopts::value<bool>()->default_value("true"))
    ("p,temp-files-dir", "Set directory of temp files", cxxopts::value<std::string>()->default_value(exePath))
    ;
    // hidden options
    options.add_options("hiddenOpts")
    ("inOutFiles", "Input and output file names", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"inOutFiles"});
    options.positional_help("input.nc output.nc");
    // 解析
    std::unique_ptr<cxxopts::ParseResult> result;
    
    try {
        result = std::make_unique<cxxopts::ParseResult>(options.parse(argc, argv));
    } catch (cxxopts::OptionParseException &e) {
        abortWithMsg(e.what());
    } catch (std::exception &e) {
        abortWithMsg("Unhandled exception!\nException message:\n"+std::string(e.what()));
    }

//    handleHelpAndVersion(result);
    if (result->count("help")) {
        std::cout << options.help({""}) << std::endl;
        exit(0);
    }
    if (result->count("version")) {
        std::cout << "TC_Tracker_CLI v0.1" << std::endl;
        exit(0);
    }
    // 处理输入输出文件
    std::vector<std::string> allFilesName{};
    try {
        allFilesName = (*result)["inOutFiles"].as<std::vector<std::string>>();
    } catch (std::domain_error) {         // 没有positional arguments
        abortWithMsg("Please specify input and output files!");
    }
    handleInOutFile(allFilesName);
    
    /// 将positional arguments全部转换为绝对路径
    std::vector<std::string> allFilesPath{};
    std::transform(allFilesName.cbegin(), allFilesName.cend(), std::back_inserter(allFilesPath), [](const std::string &fileName) {
        std::filesystem::path inFileName(fileName);
        return inFileName.is_relative() ? std::filesystem::current_path() / inFileName : inFileName;
    });
    
    /// 检查是否为wrfout文件
    auto fileInfo = TTCore::NCFileInfo(allFilesPath[0].c_str());
    std::string exceptionInfo;
    bool isWrfoutFile = fileInfo.checkIsWrfoutFile(exceptionInfo);
    std::cout << "Input file is " << (isWrfoutFile ? "" : "NOT ") << "wrfout file." << std::endl;
    VarNames varNames;
    if (isWrfoutFile) {
        varNames = VarNames("XTIME","XLAT","XLONG","vorName");
    } else {
        std::cout << exceptionInfo << std::endl;
        varNames.checkVarNames();
    }

    fileInfo = TTCore::NCFileInfo(allFilesPath[0].c_str(), isWrfoutFile, varNames, 0, (*result)["p"].as<std::string>().c_str());
    TTCore::TCs tcs;
    
    /// （无用的变量，因为cli直接Ctrl+C就可终止程序）
    bool isCanceled = false;
    fileInfo.startTracking(tcs, &isCanceled);
//    fileInfo.exportFile_nc(tcs, allFilesPath.back());
    fileInfo.exportFile_nc_compact(tcs, allFilesPath.back());
    
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
