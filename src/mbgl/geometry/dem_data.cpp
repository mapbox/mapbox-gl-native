#include <mbgl/geometry/dem_data.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

DEMData::DEMData(const PremultipliedImage& image):
    level(image.size.height, std::max<int32_t>(std::ceil(image.size.height / 2), 1)){
    if (image.size.height != image.size.width){
        throw std::runtime_error("raster-dem tiles must be square.");
    }

    for (int32_t y = 0; y < level.dim; y++) {
        for (int32_t x = 0; x < level.dim; x++) {
            const int32_t i = y * level.dim + x;
            const int32_t j = i * 4;
            level.set(x, y, (image.data[j] * 256 * 256 + image.data[j+1] * 256 + image.data[j+2])/10 - 10000);
        }
    }
    
    // in order to avoid flashing seams between tiles, here we are initially populating a 1px border of
    // pixels around the image with the data of the nearest pixel from the image. this data is eventually
    // replaced when the tile's neighboring tiles are loaded and the accurate data can be backfilled using
    // DEMData#backfillBorder

    for (int32_t x = 0; x < level.dim; x++) {
        // left vertical border
        level.set(-1, x, level.get(0, x));

        // right vertical border
        level.set(level.dim, x, level.get(level.dim - 1, x));

        //left horizontal border
        level.set(x, -1, level.get(x, 0));

        // right horizontal border
        level.set(x, level.dim, level.get(x, level.dim - 1));
    }

    // corners
    level.set(-1, -1, level.get(0, 0));
    level.set(level.dim, -1, level.get(level.dim - 1, 0));
    level.set( -1, level.dim, level.get(0, level.dim - 1));
    level.set(level.dim, level.dim, level.get(level.dim - 1, level.dim - 1));
    loaded = true;
}

void DEMData::backfillBorder(const DEMData& borderTileData, int8_t dx, int8_t dy) {
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

DEMData::Level::Level(int32_t dim_, int32_t border_)
    : dim(dim_),
      border(border_),
      stride(dim + 2 * border),
      image({ static_cast<uint32_t>(stride),
              static_cast<uint32_t>(stride) }) {
    assert(dim > 0);
    std::memset(image.data.get(), 0, image.bytes());
}

} // namespace mbgl
