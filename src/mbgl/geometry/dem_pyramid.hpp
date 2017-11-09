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
        Level(int32_t dim, size_t border);

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
            assert(x < dim + border);
            assert(y >= -border);
            assert(y < dim + border);
            return (y + border) * stride + (x + border);
        }

    public:
        const int32_t dim;
        const int32_t border;
        const int32_t stride;
        PremultipliedImage image;
    };
    
    void buildLevels();
    void loadFromImage(PremultipliedImage& image);
    void backfillBorder(DEMPyramid& borderTileData, int8_t dx, int8_t dy);

    std::vector<Level> levels;


};

} // namespace mbgl
