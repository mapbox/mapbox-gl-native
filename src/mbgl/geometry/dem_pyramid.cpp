#include <mbgl/geometry/dem_pyramid.hpp>

namespace mbgl {

void DEMPyramid::buildLevels() {
    while (true) {
        auto& prev = levels.back();
        const int32_t width = std::ceil(prev.width / 2);
        const int32_t height = std::ceil(prev.height / 2);
        const int32_t border = std::max<int32_t>(std::ceil(prev.border / 2), 1);

        if (width == 1 || height == 1) {
            break;
        }
        Level next(width, height, border);
        prev.resample(next);
        levels.emplace_back(std::move(next));
    }

    // Build remaining two levels. They aren't actually used in rendering, but we
    // need them for OpenGL's mipmapping feature.
    levels.emplace_back(2, 2, 0);
    levels.emplace_back(1, 1, 0);
}

DEMPyramid::Level::Level(int32_t width_, int32_t height_, size_t border_)
    : width(width_),
      height(height_),
      border(border_),
      stride(width + 2 * border),
      image({ static_cast<uint32_t>(width + 2 * border),
              static_cast<uint32_t>(height + 2 * border) }) {
    assert(width > 0);
    assert(height > 0);
    std::memset(image.data.get(), 0, image.bytes());
}

void DEMPyramid::Level::resample(Level& target) {
    for (int32_t y = 0; y < target.height; y++) {
        const int32_t fy = y * 2;
        for (int32_t x = 0; x < target.width; x++) {
            const int32_t fx = x * 2;
            target.set(x, y, (get(fx, fy) + get(fx + 1, fy) + get(fx, fy + 1) + get(fx + 1, fy + 1)) / 4);
        }
    }
}

} // namespace mbgl
