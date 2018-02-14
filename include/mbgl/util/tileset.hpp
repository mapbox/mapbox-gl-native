#pragma once

#include <mbgl/util/range.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/geo.hpp>
#include <tuple>
#include <vector>
#include <string>
#include <cstdint>

namespace mbgl {

class Tileset {
public:
    enum class Scheme : bool { XYZ, TMS };
    enum class DEMEncoding : bool { Mapbox, Terrarium };

    std::vector<std::string> tiles;
    Range<uint8_t> zoomRange;
    std::string attribution;
    Scheme scheme;
    // DEMEncoding is not supported by the TileJSON spec
    DEMEncoding encoding;
    optional<LatLngBounds> bounds;


    Tileset(std::vector<std::string> tiles_ = std::vector<std::string>(),
            Range<uint8_t> zoomRange_ = { 0, util::DEFAULT_MAX_ZOOM },
            std::string attribution_ = {},
            Scheme scheme_ = Scheme::XYZ,
            DEMEncoding encoding_ = DEMEncoding::Mapbox)
        : tiles(std::move(tiles_)),
          zoomRange(std::move(zoomRange_)),
          attribution(std::move(attribution_)),
          scheme(scheme_),
          encoding(encoding_),
          bounds() {};

    // TileJSON also includes center and zoom but they are not used by mbgl.

    friend bool operator==(const Tileset& lhs, const Tileset& rhs) {
        return std::tie(lhs.tiles, lhs.zoomRange, lhs.attribution, lhs.scheme, lhs.bounds)
            == std::tie(rhs.tiles, rhs.zoomRange, rhs.attribution, rhs.scheme, rhs.bounds);
    }

    friend bool operator!=(const Tileset& lhs, const Tileset& rhs) {
        return !(lhs == rhs);
    }
};

} // namespace mbgl
