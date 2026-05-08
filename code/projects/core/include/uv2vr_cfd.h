//
//  uv2vr_cfd.h
//  calRelativeVor_cxx
//
//  Created on 2021/10/30.
//

#pragma once

class uv2vr_cfd {

public:
    bool calRV(const float* const u, const float* const v, const float* const latData, const float* const lonData, int latSize, int lonSize, float msgValue, int iopt, float* const rv);
private:
    void toAverage(float* const data, int dataSize, float msgValue);
    void extrapolateForCorner(float* const data, int latSize, int lonSize, float msgValue);
    
};
