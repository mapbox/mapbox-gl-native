#pragma once

#include <mbgl/util/range.hpp>
#include <mbgl/util/constants.hpp>

#include <tuple>
#include <vector>
#include <string>
#include <cstdint>

namespace mbgl {

class Tileset {
public:
    enum class Scheme : bool { XYZ, TMS };

    std::vector<std::string> tiles;
    Range<uint8_t> zoomRange;
    std::string attribution;
    Scheme scheme;

    Tileset(std::vector<std::string> tiles_ = std::vector<std::string>(),
            Range<uint8_t> zoomRange_ = { 0, util::DEFAULT_MAX_ZOOM },
            std::string attribution_ = {},
            Scheme scheme_ = Scheme::XYZ)
        : tiles(std::move(tiles_)),
          zoomRange(std::move(zoomRange_)),
          attribution(std::move(attribution_)),
          scheme(scheme_) {}

    // TileJSON also includes center, zoom, and bounds, but they are not used by mbgl.

    friend bool operator==(const Tileset& lhs, const Tileset& rhs) {
        return std::tie(lhs.tiles, lhs.zoomRange, lhs.attribution, lhs.scheme)
            == std::tie(rhs.tiles, rhs.zoomRange, rhs.attribution, rhs.scheme);
    }
};

} // namespace mbgl
