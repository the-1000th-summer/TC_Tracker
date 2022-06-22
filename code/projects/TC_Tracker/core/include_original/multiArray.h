#pragma once

#include <cstddef>
#include <memory>
#include <algorithm>
#include <numeric>
#include <utility>
#include <cassert>

#ifdef _OPENMP
  #include <omp.h>
#endif

namespace TTCore {

class TwoDArray {
    size_t _rows;
    size_t _columns;
    std::unique_ptr<float[]> data;
public:
    TwoDArray(size_t rows, size_t columns) : _rows{rows}, _columns{columns}, data{std::make_unique<float[]>(rows * columns)} {}
    
    inline size_t rows() const { return _rows; }
    inline size_t columns() const { return _columns; }
    
    float *operator[](size_t row) { return row * _columns + data.get(); }
    inline float &operator()(size_t row, size_t column) {
        return data[row * _columns + column];
    }
    inline float *get() {
        return data.get();
    }
    std::pair<std::pair<int, int>, float> max() const {
        auto maxElemIter = std::max_element(data.get(), data.get() + _rows*_columns);
        auto maxElemIndex = std::distance(data.get(), maxElemIter);
        return {{maxElemIndex/_columns, maxElemIndex%_columns}, *maxElemIter};
    }
    std::pair<std::pair<int, int>, float> min() const {
        auto minElemIter = std::min_element(data.get(), data.get() + _rows*_columns);
        auto minElemIndex = std::distance(data.get(), minElemIter);
        return {{minElemIndex/_columns, minElemIndex%_columns}, *minElemIter};
    }
    // void printData() {
    //     for (int i = 0; i < 2; i++) {
    //         for (int j = 0; j < _columns; ++j) {
    //             cout << operator()(i,j) << " ";
    //         }
    //         cout << endl;
    //     }
    // }
};

class ThreeDArray {
    size_t _times;
    size_t _rows;
    size_t _columns;
    std::unique_ptr<float[]> data;
    /// 是否已经初始化
    bool inited = false;
public:
    ThreeDArray(size_t times, size_t rows, size_t columns) : _times{times}, _rows{rows}, _columns{columns}, data{std::make_unique<float[]>(times * rows * columns)} { inited = true; }
    
    ThreeDArray() {}
    inline void setDims(size_t times, size_t rows, size_t columns) {
        assert(inited == false);
        _times = times; _rows = rows; _columns = columns;
        data = std::make_unique<float[]>(times * rows * columns);
    }
    
    inline size_t times() const { return _times; }
    inline size_t rows() const { return _rows; }
    inline size_t columns() const { return _columns; }
    
    float *operator[](size_t timeIndex) { return timeIndex*_rows*_columns + data.get(); }
    inline float &operator()(size_t timeIndex, size_t row, size_t column) {
        return data[timeIndex*_rows*_columns + row*_columns + column];
    }
    inline float *get() {
        return data.get();
    }
    /// 以指定的值填充array中所有的element
    /// @param value 要填充的值
    inline void fillWith(float value) {
        std::fill_n(data.get(), _times*_rows*_columns, value);
    }
    std::pair<std::tuple<int, int, int>, float> max() const {
        auto maxElemIter = std::max_element(data.get(), data.get() + _times*_rows*_columns);
        auto maxElemIndex = std::distance(data.get(), maxElemIter);
        return {{maxElemIndex/(_rows*_columns), maxElemIndex%(_rows*_columns)/_columns, maxElemIndex%_columns}, *maxElemIter};
    }
    /// 计算出最高维为index时二维场的最大值
    /// @param index 最高维的index
    std::pair<std::pair<int, int>, float> max(size_t index) const {
        auto maxElemIter = std::max_element(data.get()+index*_rows*_columns, data.get()+(index+1)*_rows*_columns);
        auto maxElemIndex = std::distance(data.get()+index*_rows*_columns, maxElemIter);
        return {{maxElemIndex/_columns, maxElemIndex%_columns}, *maxElemIter};
    }
    std::pair<std::tuple<int, int, int>, float> min() const {
        auto minElemIter = std::min_element(data.get(), data.get() + _times*_rows*_columns);
        auto minElemIndex = std::distance(data.get(), minElemIter);
        return {{minElemIndex/(_rows*_columns), minElemIndex%(_rows*_columns)/_columns, minElemIndex%_columns}, *minElemIter};
    }
    float avgMinValue(int threadNum) const {
//        float avgValue = 0;
        auto avgValues = std::make_unique<float[]>(_times);
#ifdef _OPENMP
#       pragma omp parallel for num_threads(threadNum)
#endif
        for (size_t i = 0; i < _times; ++i) {
//            avgValue += (*std::min_element(i*_rows*_columns + data.get(), (i+1)*_rows*_columns + data.get()));
            avgValues[i] = (*std::min_element(i*_rows*_columns + data.get(), (i+1)*_rows*_columns + data.get()));
        }
//        return avgValue / _times;
        return std::accumulate(avgValues.get(), avgValues.get()+_times, 0.0) / static_cast<float>(_times);
    }
    // void printData() {
    //     for (int i = 0; i < 2; i++) {
    //         for (int j = 0; j < _columns; ++j) {
    //             cout << operator()(i,j) << " ";
    //         }
    //         cout << endl;
    //     }
    // }
};

}
