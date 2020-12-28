#include <mbgl/geometry/dem_data.hpp>
#include <mbgl/math/clamp.hpp>

namespace mbgl {

DEMData::DEMData(const PremultipliedImage& _image, Tileset::DEMEncoding _encoding):
    dim(_image.size.height),
    // extra two pixels per row for border backfilling on either edge
    stride(dim + 2),
    encoding(_encoding),
    image({ static_cast<uint32_t>(stride), static_cast<uint32_t>(stride) }) {

    if (_image.size.height != _image.size.width){
        throw std::runtime_error("raster-dem tiles must be square.");
    }

    auto* dest = reinterpret_cast<uint32_t*>(image.data.get()) + stride + 1;
    auto* source = reinterpret_cast<uint32_t*>(_image.data.get());
    for (int32_t y = 0; y < dim; y++) {
        memcpy(dest, source, dim * 4);
        dest += stride;
        source += dim;
    }
    
    // in order to avoid flashing seams between tiles, here we are initially populating a 1px border of
    // pixels around the image with the data of the nearest pixel from the image. this data is eventually
    // replaced when the tile's neighboring tiles are loaded and the accurate data can be backfilled using
    // DEMData#backfillBorder

    auto* data = reinterpret_cast<uint32_t*>(image.data.get());
    for (int32_t x = 0; x < dim; x++) {
        auto rowOffset = stride * (x + 1);
        // left vertical border
        data[rowOffset] = data[rowOffset + 1];

        // right vertical border
        data[rowOffset + dim + 1] = data[rowOffset + dim];
    }
    
    // top horizontal border with corners
    memcpy(data, data + stride, stride * 4);
    // bottom horizontal border with corners
    memcpy(data + (dim + 1) * stride, data + dim * stride, stride * 4);
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
    int32_t xMin = dx * dim;
    int32_t xMax = dx * dim + dim;
    int32_t yMin = dy * dim;
    int32_t yMax = dy * dim + dim;
    
    if (dx == -1) xMin = xMax - 1;
    else if (dx == 1) xMax = xMin + 1;
    
    if (dy == -1) yMin = yMax - 1;
    else if (dy == 1) yMax = yMin + 1;
    
    int32_t ox = -dx * dim;
    int32_t oy = -dy * dim;
    
    auto* dest = reinterpret_cast<uint32_t*>(image.data.get());
    auto* source = reinterpret_cast<uint32_t*>(o.image.data.get());

    for (int32_t y = yMin; y < yMax; y++) {
        for (int32_t x = xMin; x < xMax; x++) {
            dest[idx(x, y)] = source[idx(x + ox, y + oy)];
        }
    }
}

int32_t DEMData::get(const int32_t x, const int32_t y) const {
    const auto& unpack = getUnpackVector();
    const uint8_t* value = image.data.get() + idx(x, y) * 4;
    return value[0] * unpack[0] + value[1] * unpack[1] + value[2] * unpack[2] - unpack[3];
}

const std::array<float, 4>& DEMData::getUnpackVector() const {
    // https://www.mapbox.com/help/access-elevation-data/#mapbox-terrain-rgb
    static const std::array<float, 4> unpackMapbox = {{ 6553.6, 25.6, 0.1, 10000.0 }};
    // https://aws.amazon.com/public-datasets/terrain/
    static const std::array<float, 4> unpackTerrarium = {{ 256.0, 1.0, 1.0 / 256.0, 32768.0 }};

    return encoding == Tileset::DEMEncoding::Terrarium ? unpackTerrarium : unpackMapbox;
}

} // namespace mbgl
