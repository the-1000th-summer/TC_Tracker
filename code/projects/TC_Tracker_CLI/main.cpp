//
//  main.cpp
//  TC_Tracker_CLI
//
//  Created by the-1000th-summer on 2021/9/14.
//
#include <iostream>
#include <netcdf>
#include <filesystem>
// #ifdef _WIN32
// #include <windows.h>
// #endif
//#include <numeric>
//#include <boost/program_options.hpp>
#include "cxxopts.hpp"
#include "NCFileInfo.h"
#include "Typhoon.h"
#include "Utils.h"

//namespace bpo = boost::program_options;

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

// void configureConsoleEncoding() {
// #ifdef _WIN32
//     SetConsoleOutputCP(CP_UTF8);
//     SetConsoleCP(CP_UTF8);
// #endif
// }

inline void abortWithMsg(const std::string &msg) {
    std::cout << "\nERROR!" << std::endl;
    std::cout << msg << std::endl;
    std::cout << "Aborted.\n" << std::endl;
    exit(1);
}

/// 拆分字符串
/// @param[in] s 原始字符串
/// @param[in] delim 分隔符
/// @returns 拆分后的字符串vector
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    if (s.back() == delim) {
        result.push_back("");
    }
    return result;
}

/// 连接字符串
/// @param[in] strVec 多个字符串组成的vector
/// @param[in] delimiter 分隔符
/// @returns 连接后的字符串
std::string joinStrings(const std::vector<std::string> &strVec, const char *delimiter) {
    std::string ret;
    auto iter = strVec.begin(), endIter = strVec.end() - 1;
    for (; iter != endIter; ++iter) {
        ret += (*iter + delimiter);
    }
    ret += *iter;
    return ret;
}

/// 处理输入输出的文件名，并打印文件名信息
/// vector的最后一个元素视为输出文件，其他元素视为输入文件
std::vector<std::string> handleInOutFile(cxxopts::ParseResult *result) {
    std::vector<std::string> allFilesName{};
    try {
        allFilesName = (*result)["inOutFiles"].as<std::vector<std::string>>();
    } catch (std::domain_error) {         // 没有positional arguments
        abortWithMsg("Please specify input and output files!");
    }
    
    if (allFilesName.size() == 1) {
        abortWithMsg("Please specify output file path!");
    }
    if (allFilesName.size() > 2) {
        abortWithMsg("Do not specify more than two file path.");
    }
    std::cout << "TC_Tracker start..." << std::endl;
    std::cout << "Input file:" << std::endl;
    for (int i = 0; i < allFilesName.size()-1; ++i) {
        std::cout << "  " << allFilesName[i] << std::endl;
    }
    std::cout << "Output file:\n  " << allFilesName.back() << std::endl;
    
    /// 将positional arguments全部转换为绝对路径
    std::vector<std::string> allFilesPath{};
    std::transform(allFilesName.cbegin(), allFilesName.cend(), std::back_inserter(allFilesPath), [](const std::string &fileName) {
        std::filesystem::path inFileName(fileName);

        auto fullPath = inFileName.is_relative() ? std::filesystem::current_path() / inFileName : inFileName;
        return fullPath.string();
    });
    return allFilesPath;
}

bool checkIsWrfoutFile(const std::string &inFilePath) {
    auto fileInfo = TTCore::NCFileInfo(inFilePath.c_str());
    std::string exceptionInfo;
    bool isWrfoutFile = fileInfo.checkIfIsWrfoutFile(exceptionInfo);
    std::cout << "Input file is " << (isWrfoutFile ? "" : "NOT ") << "wrfout file." << std::endl;
    return isWrfoutFile;
}

VarNames getVarNames(const cxxopts::ParseResult *result, bool isWrfoutFile) {
    
    VarNames varNames;
    if (isWrfoutFile) {
        varNames = VarNames("XTIME","XLAT","XLONG","","U","V",false);
    } else if (result->count("n")) {
        std::cout << (*result)["n"].as<std::string>() << std::endl;
        auto varNamesVec = split((*result)["n"].as<std::string>(), ',');
        if (varNamesVec.size() != 6) { abortWithMsg("Must specify time,lat,lon,vorticity,uwnd,vwnd variable names in order, and use \",\" as delimiter!"); }
        if (varNamesVec[3].empty() && varNamesVec[4].empty()) { abortWithMsg("Must specify vorticity or wind var names!"); }
        if (!varNamesVec[3].empty() && !varNamesVec[4].empty()) { abortWithMsg("You can only specify vorticity OR wind var names!"); }
        varNames = VarNames(varNamesVec[0], varNamesVec[1], varNamesVec[2], varNamesVec[3], varNamesVec[4], varNamesVec[5], !varNamesVec[3].empty());
    } else {
//        std::cout << exceptionInfo << std::endl;
        varNames.checkVarNames();
    }
    return varNames;
}

int handleZLvIndex(const cxxopts::ParseResult *result, const VarNames &varNames, const std::string &inFilePath, bool isWrfoutFile) {
    auto fileInfo = TTCore::NCFileInfo(inFilePath.c_str(), varNames);
    fileInfo.isWrfoutFile = isWrfoutFile;
    
    std::string zLvDimName;
    int zLvNum = fileInfo.getZLvDimLenName(zLvDimName);
    
    if (!zLvNum) {
        std::cout << "No z dimension in the file." << std::endl;
        return -1;
    }
    if (result->count("z") == 1) {
        std::cout << "Only one z level in file. Use it." << std::endl;
        return 0;
    }
    if (!result->count("z"))
        abortWithMsg("z level index not specified!\nPlease specify with option \"-z\"");
    
    int zLvIndex = (*result)["z"].as<int>();
    std::cout << "z level index (0-based): " << zLvIndex << std::endl;
    if (zLvIndex > zLvNum-1)
        abortWithMsg("Specified z level index is invalid!\nPlease specify between 0 and "+std::to_string(zLvNum-1));
    
    return zLvIndex;
}

/// 处理是否输出中间文件以及中间文件的路径
std::pair<bool, std::string> handleTempFiles(const cxxopts::ParseResult *result) {
    const bool noTempFiles = result->count("t") ? true : false;
    const std::string tempFilesDir = (*result)["p"].as<std::string>();
    std::cout << "Will " + std::string(noTempFiles ? "NOT " : "") + "export temp files." << std::endl;
    if (noTempFiles) {
        if (!tempFilesDir.empty())
            std::cout << "Temp files dir specified by user is ignored." << std::endl;
    } else {
        if (result->count("p")) {
            std::cout << "Temp files directory: " + tempFilesDir << std::endl;
        } else {
            std::cout << "Not specified temp files dir, temp files will be saved to executable file folder: " << std::endl;
        }
    }
    return {noTempFiles, tempFilesDir};
}

int handleThreadNum(const cxxopts::ParseResult *result) {
    if (!result->count("thread"))
        return 1;
    int threadNum = (*result)["thread"].as<int>();
    if (threadNum < 0)
        abortWithMsg("Thread number cannot be negative!");
    // 将判断是否为0留到Processor.cpp中
    return threadNum;
}

double handleRegrid(const cxxopts::ParseResult *result) {
    if (!result->count("r")) {
        std::cout << "Data will not be regridded." << std::endl;
        return 0.0;
    }
    double toGridRes = (*result)["r"].as<double>();
    if (toGridRes <= 0.0) {
        std::cout << "Data will not be regridded." << std::endl;
        return toGridRes;
    }
    std::cout << "Data will be regridded to " << toGridRes << "°." << std::endl;
    return toGridRes;
}

void myStepPgCB(int a, void *aa) {
    
}
void myProgressCB(double a, void *aa) {
    std::cout << a << "%" << std::endl;
}


/// 从文件的绝对路径获取扩展名(包括“.”)
/// @param filePath 文件的绝对路径
std::string getExtensionName(const std::string &filePath) {
    std::filesystem::path filePathObj(filePath);
    return filePathObj.extension().string();
}

void handleOutputFileExtension(const std::string &filePath) {
    std::string fileExtensionStr = getExtensionName(filePath);
    std::vector<std::string> validExtension{".json", ".pb", ".nc"};
    if(std::find(validExtension.begin(), validExtension.end(), fileExtensionStr) == validExtension.end()) {
        abortWithMsg("Extension \"" + fileExtensionStr + "\" is not supported\n\".json\", \".pb\", \".nc\" are supported extensions.");
    }
}

void tryCXXOPTS(int argc, char * argv[]) {
    auto exePath = std::filesystem::weakly_canonical(std::filesystem::path(argv[0])).parent_path();
    std::vector<std::string> argList(argv, argv + argc);   // for history

    cxxopts::Options options("TC_Tracker_CLI", "热带气旋路径识别软件(TC Tracker): Tropical Cyclone tracking program");
    options.add_options()
    ("c,compact-nc-file", "The program will output compact version of netCDF file")
    ("h,help", "Display available options") // a bool parameter
    ("v,version", "Print version information")
    ("z,z-lv-index", "Specify the index of z level (0-based)", cxxopts::value<int>())
    ("r,to-grid-res", "Specify regrid resolution. Will not regrid if set this flag <= 0.", cxxopts::value<double>()->default_value("0.0"))
    ("t,no-temp-files", "Do not export temp files")
    ("n,var-names", "Set time,lat,lon,vorticity,uwnd,vwnd variable names. \",\" as separator.", cxxopts::value<std::string>())
    ("thread", "Set the number of threads tracking, 0 for maximum number of threads", cxxopts::value<int>()->default_value("1"))
    ("p,temp-files-dir", "Set directory of temp files", cxxopts::value<std::string>()->default_value(exePath.string()))
    ("min-vorticity", "Set vorticity threshold", cxxopts::value<float>())
    ;
    // hidden options
    options.add_options("hiddenOpts")
    ("inOutFiles", "Input and output file names", cxxopts::value<std::vector<std::string>>());

    options.parse_positional({"inOutFiles"});
    options.positional_help("input.nc output.[json | pb | nc]");
    // 解析
    std::unique_ptr<cxxopts::ParseResult> result;
    
    try {
        result = std::make_unique<cxxopts::ParseResult>(options.parse(argc, argv));
    } catch (cxxopts::OptionParseException &e) {
        abortWithMsg(e.what());
    } catch (std::exception &e) {
        abortWithMsg("Unhandled exception!\nException message:\n" + std::string(e.what()));
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

    auto allFilesPath = handleInOutFile(result.get());
    
    std::string outFilePath = allFilesPath.back();
    handleOutputFileExtension(outFilePath);
    
    bool isWrfoutFile = checkIsWrfoutFile(allFilesPath[0]);
    auto varNames = getVarNames(result.get(), isWrfoutFile);
    int zLvIndex = handleZLvIndex(result.get(), varNames, allFilesPath[0], isWrfoutFile);
    auto [noTempFiles, tempFilesDir] = handleTempFiles(result.get());
    int threadNum = handleThreadNum(result.get());
    double toGridRes = handleRegrid(result.get());
    
    auto fileInfo = TTCore::NCFileInfo(allFilesPath[0].c_str(), isWrfoutFile, varNames, zLvIndex, toGridRes, noTempFiles, threadNum, tempFilesDir.c_str(), "");
    TTCore::TCs tcs;
    
    /// （无用的变量，因为cli直接Ctrl+C就可终止程序）
    bool shouldCancel = false;
    fileInfo.startTracking(tcs, myStepPgCB, myProgressCB, nullptr, &shouldCancel);
    
    /// 检查输出multidimensional的nc文件还是jagged array nc文件
    std::string argStr = joinStrings(argList, " ");
    
    
    std::string fileExtensionStr = getExtensionName(outFilePath);
    if (fileExtensionStr == ".json") {
        if (result->count("c")) {
            std::cout << "Warning: export as compact version does not support in json file." << std::endl;
        }
        fileInfo.exportFile_json(tcs, outFilePath);
    } else if (fileExtensionStr == ".pb") {
        if (result->count("c")) {
            std::cout << "Warning: export as compact version does not support in protobuf file." << std::endl;
        }
        fileInfo.exportFile_proto3(tcs, outFilePath);
    } else if (fileExtensionStr == ".nc") {
        if (result->count("c")) {
            fileInfo.exportFile_nc_compact(tcs, outFilePath, argStr);
        } else {
            fileInfo.exportFile_nc(tcs, outFilePath, argStr);
        }
    }
    
//    auto pp = (std::filesystem::path(allFilesPath.back()).parent_path() / (std::filesystem::path(allFilesPath.back()).stem().string() + ".protobuf")).string();
//    fileInfo.exportFile_proto3(tcs, pp);
    
    std::cout << "end of tracking" << std::endl;
}

//void tryBoostOptions(int argc, char * argv[]) {
//    bpo::options_description desc("Allowed options");
//    desc.add_options()
//        ("help,h", "Display available options")
//        ("version,v", "Print version information")
//        ("compression", bpo::value<int>(), "set compression level");
//    bpo::variables_map vm;
//    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
//    bpo::notify(vm);
//
//    if (vm.count("help")) {
//        std::cout << desc << std::endl;
//        return;
//    }
//    if (vm.count("version")) {
//        std::cout << "TC_Tracker_CLI v0.1" << std::endl;
//        return;
//    }
//
//    if (vm.count("compression")) {
//        std::cout << "Compression level was set to "
//     << vm["compression"].as<int>() << ".\n";
//    } else {
//        std::cout << "Compression level was not set.\n";
//    }
//
//}


int main(int argc, char * argv[]) {
    // configureConsoleEncoding();
    
//    netCDF::NcFile("dsf", netCDF::NcFile::read);
    tryCXXOPTS(argc, argv);
//    tryBoostOptions(argc, argv);
    std::cout << "done!" << std::endl;
}
