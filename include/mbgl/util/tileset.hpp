#pragma once

#include <mbgl/util/range.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace mbgl {

class Tileset {
public:
    enum class Scheme : bool { XYZ, TMS };

    std::vector<std::string> tiles;
    Range<uint8_t> zoomRange { 0, 22 };
    std::string attribution;
    Scheme scheme = Scheme::XYZ;

    // TileJSON also includes center, zoom, and bounds, but they are not used by mbgl.
};

} // namespace mbgl
