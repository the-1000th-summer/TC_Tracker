#pragma once

#include <cstddef>
#include <memory>
#include <algorithm>
#include <utility>


class TwoDArray {
    size_t _rows;
    size_t _columns;
    std::unique_ptr<float[]> data;
public:
    TwoDArray(size_t rows, size_t columns) : _rows{rows}, _columns{columns}, data{std::make_unique<float[]>(rows * columns)} {}

    inline size_t rows() const { return _rows; }
    inline size_t columns() const { return _columns; }
    
    // int *operator[](size_t row) { return row * _columns + data.get(); }
    inline float &operator()(size_t row, size_t column) {
        return data[row * _columns + column];
    }
    inline float *get() {
        return data.get();
    }
    std::pair<std::pair<int, int>, float> max() {
        auto maxElemIter = std::max_element(data.get(), data.get() + _rows*_columns);
        auto maxElemIndex = std::distance(data.get(), maxElemIter);
        return {{maxElemIndex/_columns, maxElemIndex%_columns}, *maxElemIter};
    }
    std::pair<std::pair<int, int>, float> min() {
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