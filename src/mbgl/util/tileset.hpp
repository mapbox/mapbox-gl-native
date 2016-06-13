#pragma once

#include <mbgl/util/constants.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/util/geo.hpp>

#include <array>
#include <vector>
#include <string>
#include <cstdint>

namespace mbgl {

class Tileset {
public:
    std::vector<std::string> tiles;
    Range<uint8_t> zoomRange { 0, 22 };
    std::string attribution;
    LatLng center;
    double zoom = 0;
    LatLngBounds bounds = LatLngBounds::world();
};

} // namespace mbgl
