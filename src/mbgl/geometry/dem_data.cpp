#include <mbgl/geometry/dem_data.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

DEMData::DEMData(const PremultipliedImage& _image, Tileset::DEMEncoding encoding):
    dim(_image.size.height),
    border(std::max<int32_t>(std::ceil(_image.size.height / 2), 1)),
    stride(dim + 2 * border),
    image({ static_cast<uint32_t>(stride), static_cast<uint32_t>(stride) }) {

    if (_image.size.height != _image.size.width){
        throw std::runtime_error("raster-dem tiles must be square.");
    }

    auto decodeMapbox = [] (const uint8_t r, const uint8_t g, const uint8_t b){
        // https://www.mapbox.com/help/access-elevation-data/#mapbox-terrain-rgb
        return (r * 256 * 256 + g * 256 + b)/10 - 10000;
    };

    auto decodeTerrarium = [] (const uint8_t r, const uint8_t g, const uint8_t b){
        // https://aws.amazon.com/public-datasets/terrain/
        return ((r * 256 + g + b / 256) - 32768);
    };

    auto decodeRGB = encoding == Tileset::DEMEncoding::Terrarium ? decodeTerrarium : decodeMapbox;

    std::memset(image.data.get(), 0, image.bytes());

    for (int32_t y = 0; y < dim; y++) {
        for (int32_t x = 0; x < dim; x++) {
            const int32_t i = y * dim + x;
            const int32_t j = i * 4;
            set(x, y, decodeRGB(_image.data[j], _image.data[j+1], _image.data[j+2]));
        }
    }
    
    // in order to avoid flashing seams between tiles, here we are initially populating a 1px border of
    // pixels around the image with the data of the nearest pixel from the image. this data is eventually
    // replaced when the tile's neighboring tiles are loaded and the accurate data can be backfilled using
    // DEMData#backfillBorder

    for (int32_t x = 0; x < dim; x++) {
        // left vertical border
        set(-1, x, get(0, x));

        // right vertical border
        set(dim, x, get(dim - 1, x));

        //left horizontal border
        set(x, -1, get(x, 0));

        // right horizontal border
        set(x, dim, get(x, dim - 1));
    }

    // corners
    set(-1, -1, get(0, 0));
    set(dim, -1, get(dim - 1, 0));
    set( -1, dim, get(0, dim - 1));
    set(dim, dim, get(dim - 1, dim - 1));
}

// This function takes the DEMData from a neighboring tile and backfills the edge/corner
// data in order to create a one pixel "buffer" of image data around the tile. This is
// necessary because the hillshade formula calculates the dx/dz, dy/dz derivatives at each
// pixel of the tile by querying the 8 surrounding pixels, and if we don't have the pixel
// buffer we get seams at tile boundaries.
void DEMData::backfillBorder(const DEMData& borderTileData, int8_t dx, int8_t dy) {
    auto& o = borderTileData;

    // Tiles from the same source should always be of the same dimensions.
    assert(dim == o.dim);

    // We determine the pixel range to backfill based which corner/edge `borderTileData`
    // represents. For example, dx = -1, dy = -1 represents the upper left corner of the
    // base tile, so we only need to backfill one pixel at coordinates (-1, -1) of the tile
    // image.
    int32_t _xMin = dx * dim;
    int32_t _xMax = dx * dim + dim;
    int32_t _yMin = dy * dim;
    int32_t _yMax = dy * dim + dim;
    
    if (dx == -1) _xMin = _xMax - 1;
    else if (dx == 1) _xMax = _xMin + 1;
    
    if (dy == -1) _yMin = _yMax - 1;
    else if (dy == 1) _yMax = _yMin + 1;
    
    int32_t xMin = util::clamp(_xMin, -border, dim + border);
    int32_t xMax = util::clamp(_xMax, -border, dim + border);
    
    int32_t yMin = util::clamp(_yMin, -border, dim + border);
    int32_t yMax = util::clamp(_yMax, -border, dim + border);
    
    int32_t ox = -dx * dim;
    int32_t oy = -dy * dim;
    
    for (int32_t y = yMin; y < yMax; y++) {
        for (int32_t x = xMin; x < xMax; x++) {
            set(x, y, o.get(x + ox, y + oy));
        }
    }
}

} // namespace mbgl
