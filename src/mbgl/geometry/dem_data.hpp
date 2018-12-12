#pragma once

#include <mbgl/math/clamp.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/tileset.hpp>

#include <memory>
#include <array>
#include <cassert>
#include <vector>

namespace mbgl {

class DEMData {
public:
    DEMData(const PremultipliedImage& image, Tileset::DEMEncoding encoding);
    void backfillBorder(const DEMData& borderTileData, int8_t dx, int8_t dy);

    void set(const int32_t x, const int32_t y, const int32_t value) {
        reinterpret_cast<int32_t*>(image.data.get())[idx(x, y)] = value + 65536;
    }

    int32_t get(const int32_t x, const int32_t y) const {
        return reinterpret_cast<const int32_t*>(image.data.get())[idx(x, y)] - 65536;
    }

    const PremultipliedImage* getImage() const {
        return &image;
    }

    const int32_t dim;
    const int32_t stride;


    private:
        PremultipliedImage image;

        size_t idx(const int32_t x, const int32_t y) const {
            assert(x >= -1);
            assert(x < dim + 1);
            assert(y >= -1);
            assert(y < dim + 1);
            return (y + 1) * stride + (x + 1);
        }

};

} // namespace mbgl
