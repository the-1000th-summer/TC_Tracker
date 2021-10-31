//
//  uv2vr_cfd.h
//  calRelativeVor_cxx
//
//  Created by 邓浩 on 2021/10/30.
//

#pragma once
#include "multiArray.h"

class uv2vr_cfd {

public:
    bool calRV(float *u, TTCore::TwoDArray &v, float *latData, float *lonData, int latSize, int lonSize, float msgValue, int iopt, TTCore::TwoDArray &rv);
private:
    void toAverage(float *data, int dataSize, float msgValue);
    void extrapolateForCorner(float *data, int latSize, int lonSize, float msgValue);
    
};
