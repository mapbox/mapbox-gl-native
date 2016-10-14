#pragma once

#include <mbgl/math/clamp.hpp>
#include <mbgl/util/image.hpp>

#include <memory>
#include <array>
#include <cassert>
#include <vector>

namespace mbgl {

class DEMPyramid {
public:
    class Level {
    public:
        Level(int32_t width, int32_t height, size_t border);

        void set(const int32_t x, const int32_t y, const int32_t value) {
            reinterpret_cast<int32_t*>(image.data.get())[idx(x, y)] = value + 65536;
        }

        int32_t get(const int32_t x, const int32_t y) const {
            return reinterpret_cast<const int32_t*>(image.data.get())[idx(x, y)] - 65536;
        }

        void resample(Level& target);

    private:
        size_t idx(const int32_t x, const int32_t y) const {
            assert(x >= -border);
            assert(x < width + border);
            assert(y >= -border);
            assert(y < height + border);
            return (y + border) * stride + (x + border);
        }

    public:
        const int32_t width;
        const int32_t height;
        const int32_t border;
        const int32_t stride;
        PremultipliedImage image;
    };


    void buildLevels();

    std::vector<Level> levels;


};

} // namespace mbgl
