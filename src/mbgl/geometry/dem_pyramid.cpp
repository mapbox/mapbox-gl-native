#include <mbgl/geometry/dem_pyramid.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

DEMPyramid::DEMPyramid(PremultipliedImage& image):
    level(image.size.height, std::max<int32_t>(std::ceil(image.size.height / 2), 1)){
    assert(image.size.height == image.size.width);

    for (int32_t y = 0; y < level.dim; y++) {
        for (int32_t x = 0; x < level.dim; x++) {
            const int32_t i = y * level.dim + x;
            const int32_t j = i * 4;
            level.set(x, y, (image.data[j] * 256 * 256 + image.data[j+1] * 256 + image.data[j+2])/10 - 10000);
        }
    }
    
    //    // in order to avoid flashing seams between tiles, here we are initially populating a 1px border of
    //    // pixels around the image with the data of the nearest pixel from the image. this data is eventually
    //    // replaced when the tile's neighboring tiles are loaded and the accurate data can be backfilled using
    //    // DEMPyramid#backfillBorder
    //
    ////    for (int32_t x = 0; x < first.dim; x++) {
    ////        // left vertical border
    ////        first.set(-1, x, first.get(0, x));
    ////
    ////        // right vertical border
    ////        first.set(first.dim, x, first.get(first.dim - 1, x));
    ////
    ////        //left horizontal border
    ////        first.set(x, -1, first.get(x, 0));
    ////
    ////        // right horizontal border
    ////        first.set(x, first.dim, first.get(x, first.dim - 1));
    ////    }
    ////
    ////    // corners
    ////    first.set(-1, -1, first.get(0, 0));
    ////    first.set(first.dim, -1, first.get(first.dim - 1, 0));
    ////    first.set( -1, first.dim, first.get(0, first.dim - 1));
    ////    first.set(first.dim, first.dim, first.get(first.dim - 1, first.dim - 1));
    loaded = true;
}

void DEMPyramid::backfillBorder(mbgl::DEMPyramid &borderTileData, int8_t dx, int8_t dy) {
    auto& t = level;
    auto& o = borderTileData.level;
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

DEMPyramid::Level::Level(int32_t dim_, int32_t border_)
    : dim(dim_),
      border(border_),
      stride(dim + 2 * border),
      image({ static_cast<uint32_t>(stride),
              static_cast<uint32_t>(stride) }) {
    assert(dim > 0);
    std::memset(image.data.get(), 0, image.bytes());
}

} // namespace mbgl
