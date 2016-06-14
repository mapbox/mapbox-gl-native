#pragma once

#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>

#include <array>
#include <vector>
#include <string>
#include <cstdint>

namespace mbgl {

class Tileset {
public:
    std::vector<std::string> tiles;
    uint8_t minZoom = 0;
    uint8_t maxZoom = 22;
    std::string attribution;
    LatLng center;
    double zoom = 0;
    LatLngBounds bounds = LatLngBounds::world();
};

} // namespace mbgl
