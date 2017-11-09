#include <mbgl/geometry/dem_pyramid.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

void DEMPyramid::buildLevels() {
    while (true) {
        auto& prev = levels.back();
        const int32_t dim = std::ceil(prev.dim / 2);
        const int32_t border = std::max<int32_t>(std::ceil(prev.border / 2), 1);

        if (dim == 1) {
            break;
        }
        Level next(dim, border);
        prev.resample(next);
        levels.emplace_back(std::move(next));
    }

    // Build remaining two levels. They aren't actually used in rendering, but we
    // need them for OpenGL's mipmapping feature.
    levels.emplace_back(2, 2, 0);
    levels.emplace_back(1, 1, 0);
}

void DEMPyramid::loadFromImage(PremultipliedImage& image){
    assert(image.size.height == image.size.width);
    
    Level first(image.size.height, image.size.height/2.0);
    
    for (int32_t y = 0; y < first.dim; y++) {
        for (int32_t x = 0; x < first.dim; x++) {
            const int32_t i = y * first.dim + x;
            const int32_t j = i * 4;
            first.set(x, y, (image.data[j] * 256 * 256 + image.data[j+i] * 256 + image.data[j+2])/10 - 10000);
        }
    }
    
    // in order to avoid flashing seams between tiles, here we are initially populating a 1px border of
    // pixels around the image with the data of the nearest pixel from the image. this data is eventually
    // replaced when the tile's neighboring tiles are loaded and the accurate data can be backfilled using
    // DEMPyramid#backfillBorder
    
    for (int32_t x = 0; x < first.dim; x++) {
        // left vertical border
        first.set(-1, x, first.get(0, x));
        
        // right vertical border
        first.set(first.dim, x, first.get(first.dim - 1, x));
        
        //left horizontal border
        first.set(x, -1, first.get(x, 0));
        
        // right horizontal border
        first.set(x, first.dim, first.get(x, first.dim - 1));
    }
    
    // corners
    first.set(-1, -1, first.get(0, 0));
    first.set(first.dim, -1, first.get(first.dim - 1, 0));
    first.set( -1, first.dim, first.get(0, first.dim - 1));
    first.set(first.dim, first.dim, first.get(first.dim - 1, first.dim - 1));
    
    levels.emplace_back(std::move(first));
}

void DEMPyramid::backfillBorder(mbgl::DEMPyramid &borderTileData, int8_t dx, int8_t dy) {
    auto& t = levels.front();
    auto& o = borderTileData.levels.front();
    assert(t.dim == o.dim);

    int32_t _xMin = dx * t.dim;
    int32_t _xMax = dx * t.dim + t.dim;
    int32_t _yMin = dy * t.dim;
    int32_t _yMax = dy * t.dim + t.dim;
    
    if (dx == -1) _xMin = _xMax - 1;
    else if (dx == 1) _xMax = _xMin + 1;
    
    if (dy == -1) _yMin = _yMax - 1;
    else if (dy == 1) _yMax = _yMin + 1;
    
    int32_t xMin = util::clamp(_xMin, -t.border, t.dim + t.border);
    int32_t xMax = util::clamp(_xMax, -t.border, t.dim + t.border);
    
    int32_t yMin = util::clamp(_yMin, -t.border, t.dim + t.border);
    int32_t yMax = util::clamp(_yMax, -t.border, t.dim + t.border);
    
    int32_t ox = -dx * t.dim;
    int32_t oy = -dy * t.dim;
    
    for (int32_t y = yMin; y < yMax; y++) {
        for (int32_t x = xMin; x < xMax; x++) {
            t.set(x, y, o.get(x + ox, y + oy));
        }
    }
}

DEMPyramid::Level::Level(int32_t dim_, size_t border_)
    : dim(dim_),
      border(border_),
      stride(dim + 2 * border),
      image({ static_cast<uint32_t>(dim + 2 * border),
              static_cast<uint32_t>(dim + 2 * border) }) {
    assert(dim > 0);
    std::memset(image.data.get(), 0, image.bytes());
}



void DEMPyramid::Level::resample(Level& target) {
    for (int32_t y = 0; y < target.dim; y++) {
        const int32_t fy = y * 2;
        for (int32_t x = 0; x < target.dim; x++) {
            const int32_t fx = x * 2;
            target.set(x, y, (get(fx, fy) + get(fx + 1, fy) + get(fx, fy + 1) + get(fx + 1, fy + 1)) / 4);
        }
    }
}

} // namespace mbgl
