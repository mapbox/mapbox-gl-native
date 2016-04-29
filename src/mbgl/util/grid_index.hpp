#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

namespace mbgl {

template <class T>
class GridIndex {
    public:

    GridIndex(int32_t extent_, int32_t n_, int32_t padding_);

    struct BBox {
        int32_t x1;
        int32_t y1;
        int32_t x2;
        int32_t y2;
    };

    void insert(T&& t, BBox&& bbox);
    std::vector<T> query(const BBox& bbox) const;

    private:

    int32_t convertToCellCoord(int32_t x) const;

    const int32_t extent;
    const int32_t n;
    const int32_t padding;
    const int32_t d;
    const double scale;
    const int32_t min;
    const int32_t max;

    std::vector<std::pair<T, BBox>> elements;
    std::vector<std::vector<size_t>> cells;

};

}
