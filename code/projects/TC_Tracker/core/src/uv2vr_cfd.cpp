//
//  uv2vr_cfd.cpp
//  calRelativeVor_cxx
//
//  Created by 邓浩 on 2021/10/30.
//
#include <algorithm>
#include <numeric>
#include <cmath>
#include "uv2vr_cfd.h"

bool uv2vr_cfd::calRV(float *u, float *v, float *latData, float *lonData, int latSize, int lonSize, float msgValue, int iopt, float *rv) {
    if (lonSize < 1 || latSize < 1) { return false; }
    if (std::abs(latData[0]) > 90.0 || std::abs(latData[latSize-1]) > 90.0) {return false;}
    
    double re = 6.37122e6;        // 地球半径(米)
    double rad = M_PI / 180.0;
    double rcon = re * rad;
    int jopt = std::abs(iopt);
    // pre-compute cos(lat)
    auto clat = std::make_unique<double[]>(latSize);
    for (int latIndex = 0; latIndex < latSize; ++latIndex) {
        clat[latIndex] = std::cos(rad * latData[latIndex]);
    }
    // pre-compute tan(lat)/re
    auto tLatRe = std::make_unique<double[]>(latSize);
    for (int latIndex = 0; latIndex < latSize; ++latIndex) {
        if (std::abs(latData[latIndex]) < 90.0) {   //  非极点
            tLatRe[latIndex] = std::tan(rad * latData[latIndex]) / re;
        } else {               // 极点
            int secondIndex = (latData[latIndex] == 90.0) ? (latIndex-1) : (latIndex+1);
            double polat = 0.5 * (latData[latIndex] + latData[secondIndex]);
            tLatRe[latIndex] = std::tan(rad * polat) / re;
        }
    }
    
    // 输出数组初始化为缺测值
    std::fill_n(rv, latSize*lonSize, msgValue);
    // calculate "1/dy" [bot, top]
    double dyBottom = 1.0 / (rcon * (latData[1] - latData[0]));
    double dyTop = 1.0 / (rcon * (latData[latSize-1] - latData[latSize-2]));
    // calculate "1/(2*dy)"
    auto dy2 = std::make_unique<double[]>(latSize);
    for (int latIndex = 1; latIndex < latSize-1; ++latIndex) {
        dy2[latIndex] = 1.0 / (rcon * (latData[latIndex+1] - latData[latIndex-1]));
    }
    // calculate "1/dx" [left, right] and "1/(2*dx)
    double dLon = lonData[1] - lonData[0];
    double dLon2 = lonData[2] - lonData[0];
    auto dx = std::make_unique<double[]>(latSize);
    auto dx2 = std::make_unique<double[]>(latSize);
    for (int latIndex = 0; latIndex < latSize; ++latIndex) {
        if (std::abs(latData[latIndex]) != 90.0) {
            dx[latIndex] = 1.0 / (rcon * dLon * clat[latIndex]);
            dx2[latIndex] = 1.0 / (rcon * dLon2 * clat[latIndex]);
        } else {
            dx[latIndex] = 0.0;
            dx2[latIndex] = 0.0;
        }
    }
    
    // set subscript range
    int mlStart, mlEnd;
    if (jopt == 1 || jopt == 3) {          // cyclic
        mlStart = 0;
        mlEnd = lonSize;
    } else {
        mlStart = 1;
        mlEnd = lonSize - 1;
    }
    
    // longitude loop
    for (int lonIndex = mlStart; lonIndex < mlEnd; ++lonIndex) {
        // iopt = 0 or 1
        int lonIndexM1 = lonIndex - 1;
        int lonIndexP1 = lonIndex + 1;
        if (lonIndex == 0) { lonIndexM1 = lonSize-1; }   // 最左边的格点
        if (lonIndex == lonSize-1) { lonIndexP1 = 0; }   // 最右边的格点
        
        for (int latIndex = 1; latIndex < latSize-1; ++latIndex) {
            float v_lonP1 = v[latIndex*lonSize+lonIndexP1], v_lonM1 = v[latIndex*lonSize+lonIndexM1];
            float u_latP1 = u[(latIndex+1)*lonSize+lonIndex], u_latM1 = u[(latIndex-1)*lonSize+lonIndex];
            float u_here = u[latIndex*lonSize+lonIndex];
            if (v_lonP1 != msgValue && v_lonM1 != msgValue && u_latP1 != msgValue && u_latM1 != msgValue && u_here != msgValue) {
                rv[latIndex*lonSize+lonIndex] = (v_lonP1-v_lonM1)*dx2[latIndex] - (u_latP1-u_latM1)*dy2[latIndex] + u_here*tLatRe[latIndex];
            }
        }
        
        if (jopt < 2) { continue; }
        // bottom bound (nl = 1)
        float v_b_lonP1 = v[lonIndexP1], v_b_lonM1 = v[lonIndexM1];
        float u_b_latP1 = u[lonSize+lonIndex], u_b_here = u[lonIndex];
        if (v_b_lonP1 != msgValue && v_b_lonM1 != msgValue && u_b_latP1 != msgValue && u_b_here != msgValue) {
            rv[lonIndex] = (v_b_lonP1-v_b_lonM1)*dx2[0] - (u_b_latP1-u_b_here)*dyBottom + u_b_here*tLatRe[0];
        }
        // top bound (nl = nlat)
        float v_t_lonP1 = v[(latSize-1)*lonSize+lonIndexP1], v_t_lonM1 = v[(latSize-1)*lonSize+lonIndexM1];
        float u_t_here = u[(latSize-1)*lonSize+lonIndex];
        float u_t_latM1 = u[(latSize-2)*lonSize+lonIndex];
        if (v_t_lonP1 != msgValue && v_t_lonM1 != msgValue && u_t_here != msgValue && u_t_latM1 != msgValue) {
            rv[(latSize-1)*lonSize+lonIndex] = (v_t_lonP1-v_t_lonM1)*dx2[latSize-1] - (u_t_here-u_t_latM1)*dyTop + u_t_here*tLatRe[latSize-1];
        }
    }
    
    if (jopt == 2) {      // left/right bound?
        for (int latIndex = 1; latIndex < latSize - 1; ++latIndex) {
            // left bound (lonIndex = 0)
            float v_l_lonP1 = v[latIndex*lonSize+1], v_l_here = v[latIndex*lonSize];
            float u_l_latP1 = u[(latIndex+1)*lonSize], u_l_latM1 = u[(latIndex-1)*lonSize];
            float u_l_here = u[latIndex*lonSize];
            if (v_l_lonP1 != msgValue && v_l_here != msgValue && u_l_latP1 != msgValue && u_l_latM1 != msgValue && u_l_here != msgValue) {
                rv[latIndex*lonSize] = (v_l_lonP1-v_l_here)*dx[latIndex] - (u_l_latP1-u_l_latM1)*dy2[latIndex] + u_l_here*tLatRe[latIndex];
            }
            // right bound (lonIndex = lonSize - 1)
            float v_r_here = v[latIndex*lonSize+lonSize-1], v_r_lonM1 = v[latIndex*lonSize+lonSize-2];
            float u_r_latP1 = u[(latIndex+1)*lonSize+lonSize-1];
            float u_r_latM1 = u[(latIndex-1)*lonSize+lonSize-1];
            float u_r_here = u[latIndex*lonSize+lonSize-1];
            if (v_r_here != msgValue && v_r_lonM1 != msgValue && u_r_latP1 != msgValue && u_r_latM1 != msgValue && u_r_here != msgValue) {
                rv[latIndex*lonSize+lonSize-1] = (v_r_here-v_r_lonM1)*dx[latIndex] - (u_r_latP1-u_r_latM1)*dy2[latIndex] + u_r_here*tLatRe[latIndex];
            }
        }
    }
    
    // special at +/-90 use average
    if (std::abs(latData[0]) == 90.0) { toAverage(rv, lonSize, msgValue); }
    if (std::abs(latData[latSize-1]) == 90.0) { toAverage(rv+(latSize-1)*lonSize, lonSize, msgValue); }
    
    // special for corners (jopt=2 only), use linear extrapolation from two directions
    if (jopt == 2) { extrapolateForCorner(rv, latSize, lonSize, msgValue); }
   
    return true;
}

void uv2vr_cfd::extrapolateForCorner(float *data, int latSize, int lonSize, float msgValue) {
    if (data[0] == msgValue) {                // 左下角格点
        float latP1 = data[lonSize], latP2 = data[2*lonSize];
        float lonP1 = data[1], lonP2 = data[2];
        if (latP1 != msgValue && latP2 != msgValue && lonP1 != msgValue && lonP2 != msgValue) {
            data[0] = (2.0*latP1 - latP2 + 2.0*lonP1 - lonP2) * 0.5;
        }
    }
    if (data[lonSize-1] == msgValue) {               // 右下角格点
        float latP1 = data[2*lonSize-1], latP2 = data[3*lonSize-1];
        float lonM1 = data[lonSize-2], lonM2 = data[lonSize-3];
        if (latP1 != msgValue && latP2 != msgValue && lonM1 != msgValue && lonM2 != msgValue) {
            data[lonSize-1] = (2.0*latP1 - latP2 + 2.0*lonM1 - lonM2) * 0.5;
        }
    }
    if (data[(latSize-1)*lonSize] == msgValue) {          // 左上角格点
        float latM1 = data[(latSize-2)*lonSize], latM2 = data[(latSize-3)*lonSize];
        float lonP1 = data[(latSize-1)*lonSize+1], lonP2 = data[(latSize-1)*lonSize+2];
        if (latM1 != msgValue && latM2 != msgValue && lonP1 != msgValue && lonP2 != msgValue) {
            data[(latSize-1)*lonSize] = (2.0*latM1 - latM2 + 2.0*lonP1 - lonP2) * 0.5;
        }
    }
    if (data[latSize*lonSize-1] == msgValue) {     // 右上角格点
        float latM1 = data[(latSize-1)*lonSize-1], latM2 = data[(latSize-2)*lonSize-1];
        float lonM1 = data[latSize*lonSize-2], lonM2 = data[latSize*lonSize-3];
        if (latM1 != msgValue && latM2 != msgValue && lonM1 != msgValue && lonM2 != msgValue) {
            data[latSize*lonSize-1] = (2.0*latM1-latM2 + 2.0*lonM1-lonM2) * 0.5;
        }
    }
}

/// 计算1d array中所有非缺测值的平均值并写入到该1d array的每个元素
/// @param data 1d array
/// @param dataSize 1d array的元素个数
/// @param msgValue 缺测值
void uv2vr_cfd::toAverage(float *data, int dataSize, float msgValue) {
    int msgGridNum = std::count(data, data+dataSize, msgValue);
    if (msgGridNum == dataSize) { return; }
    float avgData = (std::accumulate(data, data+dataSize, 0.0) - msgValue*msgGridNum) / (dataSize - msgGridNum);
    std::fill(data, data+dataSize, avgData);
}
