#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <netcdf>
#include <memory>
#include <utility>

#include "include/multiArray.h"

template <typename T>
inline void getValAndWriteToFile(netCDF::NcVar &oVar, const netCDF::NcAtt &varAtt) {
    T val;
    varAtt.getValues(&val);
    oVar.putAtt(varAtt.getName(), varAtt.getType(), val);
}

void copyVarAtts(const netCDF::NcVar &inVar, netCDF::NcVar &outVar) {
    auto varAtts = inVar.getAtts();
    auto map_it = varAtts.cbegin();
    for (; map_it != varAtts.cend(); ++map_it) {
        // std::cout << map_it->first << std::endl;
        auto varAtt = map_it->second;
        auto varAttTypeClass = varAtt.getType().getTypeClass();
        
        switch (varAttTypeClass) {
        case netCDF::NcType::nc_INT:
            getValAndWriteToFile<int>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_FLOAT:
            getValAndWriteToFile<float>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_CHAR: case netCDF::NcType::nc_STRING: {
            std::string val;
            varAtt.getValues(val);
            outVar.putAtt(varAtt.getName(), val);
            break;
        }
        case netCDF::NcType::nc_DOUBLE:
            getValAndWriteToFile<double>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_SHORT:
            getValAndWriteToFile<short>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_BYTE:
            getValAndWriteToFile<char>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_UBYTE:
            getValAndWriteToFile<unsigned char>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_USHORT:
            getValAndWriteToFile<ushort>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_UINT:
            getValAndWriteToFile<uint>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_INT64:
            getValAndWriteToFile<long long>(outVar, varAtt);
            break;
        case netCDF::NcType::nc_UINT64:
            getValAndWriteToFile<unsigned long long>(outVar, varAtt);
            break;
        default:
            throw "unknown type!";
        }
    }
}

void computeAbsVort(netCDF::NcFile &inFile, ThreeDArray &av) {
    size_t nt = inFile.getDim("Time").getSize();
    size_t ny = inFile.getDim("south_north").getSize();
    size_t nx = inFile.getDim("west_east").getSize();
    float dx, dy;
    inFile.getAtt("DX").getValues(&dx);
    inFile.getAtt("DY").getValues(&dy);
    auto msfm = ThreeDArray(nt, ny, nx);
    inFile.getVar("MAPFAC_M").getVar(msfm.get());
    auto u = ThreeDArray(nt,ny,nx+1), msfu = ThreeDArray(nt,ny,nx+1);
    inFile.getVar("U").getVar({0,6,0,0}, {nt,1,ny,nx+1}, u.get());
    inFile.getVar("MAPFAC_U").getVar(msfu.get());
    auto v = ThreeDArray(nt,ny+1,nx), msfv = ThreeDArray(nt,ny+1,nx);
    inFile.getVar("V").getVar({0,6,0,0}, {nt,1,ny+1,nx}, v.get());
    inFile.getVar("MAPFAC_V").getVar(msfv.get());
    auto cor = ThreeDArray(nt,ny,nx);
    inFile.getVar("F").getVar(cor.get());

    for (int k = 0; k < nt; ++k) {
        for (int j = 0; j < ny; ++j) {
            // jp1: 1,2,3,4...,ny-2,ny-1,ny-1
            int jp1 = std::min(j+1, static_cast<int>(ny-1));
            // jm1: 0,0,1,2...,ny-4,ny-3,ny-2
            int jm1 = std::max(j-1, 0);
            for (int i = 0; i < nx; ++i) {
                // ip1: 1,2,3,4,...,nx-2,nx-1,nx-1
                int ip1 = std::min(i+1, static_cast<int>(nx-1));
                // im1: 0,0,1,2,...,nx-4,nx-3,nx-2
                int im1 = std::max(i-1, 0);
                
                float dsx = (ip1 - im1) * dx;  // (1,2,2,2,...,2,2,1)*dx
                float dsy = (jp1 - jm1) * dy;  // (1,2,2,2,...,2,2,1)*dy
                float mm = msfm(k,j,i) * msfm(k,j,i);

                auto dudy = 0.5 * (u(k,jp1,i)/msfu(k,jp1,i) + 
                    u(k,jp1,i+1)/msfu(k,jp1,i+1) -
                    u(k,jm1,i)/msfu(k,jm1,i) - 
                    u(k,jm1,i+1)/msfu(k,jm1,i+1)) / dsy * mm;

                auto dvdx = 0.5 * (v(k,j,ip1)/msfv(k,j,ip1)+
                    v(k,j+1,ip1)/msfv(k,j+1,ip1) -
                    v(k,j,im1)/msfv(k,j,im1) -
                    v(k,j+1,im1)/msfv(k,j+1,im1)) / dsx * mm;

                auto avort = dvdx - dudy + cor(k,j,i);
                // auto avort = dvdx - dudy;
                av(k,j,i) = avort*1.0e5;
            }
        }
    }
    std::cout << av(0,0,0) << std::endl;
    std::cout << av(1,154,0) << std::endl;
    std::cout << av(1,155,0) << std::endl;
}

void toAbsoluteVort() {
    std::string fileDir = "/mnt/e/University/TC_Tracker/data/wrfFile/";
    std::string inFilePath = fileDir + "wrfout_haimaReport_try1.nc";
    std::string outputFilePath = fileDir + "absVor_cxx.nc";

    netCDF::NcFile inFile(inFilePath, netCDF::NcFile::read);
    
    size_t time_dimSize = inFile.getDim("Time").getSize();
    size_t south_north_dimSize = inFile.getDim("south_north").getSize();
    size_t west_east_dimSize = inFile.getDim("west_east").getSize();
    // std::cout << south_north_dimSize << std::endl;
    // auto attt = vv.getAtt("FieldType");


    netCDF::NcFile outputFile(outputFilePath, netCDF::NcFile::replace);
    auto south_north_dim = outputFile.addDim("south_north", south_north_dimSize);
    auto west_east_dim = outputFile.addDim("west_east", west_east_dimSize);
    auto time_dim = outputFile.addDim("Time");

    auto XTIMEVar = outputFile.addVar("XTIME", netCDF::NcType::nc_FLOAT, time_dim);
    auto XLATVar = outputFile.addVar("XLAT", netCDF::NcType::nc_FLOAT, {time_dim,south_north_dim,west_east_dim});
    auto XLONGVar = outputFile.addVar("XLONG", netCDF::NcType::nc_FLOAT, {time_dim,south_north_dim,west_east_dim});
    auto avoVar = outputFile.addVar("avo", netCDF::NcType::nc_FLOAT, {time_dim,south_north_dim,west_east_dim});

    copyVarAtts(inFile.getVar("XTIME"), XTIMEVar);
    copyVarAtts(inFile.getVar("XLAT"), XLATVar);
    copyVarAtts(inFile.getVar("XLONG"), XLONGVar);


    auto XTIMEData = std::make_unique<float[]>(time_dimSize);
    inFile.getVar("XTIME").getVar(XTIMEData.get());
    XTIMEVar.putVar({0}, {time_dimSize}, XTIMEData.get());

    auto XLATData = std::make_unique<float[]>(time_dimSize * south_north_dimSize * west_east_dimSize);
    // float *XLATData = new float[time_dimSize][south_north_dimSize][west_east_dimSize];
    inFile.getVar("XLAT").getVar(XLATData.get());
    XLATVar.putVar(XLATData.get());

    auto XLONGData = std::make_unique<float[]>(time_dimSize * south_north_dimSize * west_east_dimSize);
    inFile.getVar("XLONG").getVar(XLONGData.get());
    XLONGVar.putVar(XLONGData.get());

    auto absVorData = ThreeDArray(time_dimSize,south_north_dimSize,west_east_dimSize);
    computeAbsVort(inFile, absVorData);
    avoVar.putVar(absVorData.get());


    // delete [] XLATData;

    outputFile.close();
}

int main(int, char**) {
    // toAbsoluteVort();
    std::pair<int, int> b = {5, 6};
    auto [a, c] = b;
    std::cout << a << c << std::endl;

}
