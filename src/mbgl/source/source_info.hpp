#ifndef MBGL_MAP_SOURCE_INFO
#define MBGL_MAP_SOURCE_INFO

#include <mbgl/style/types.hpp>
#include <mbgl/util/constants.hpp>

#include <array>
#include <vector>
#include <string>
#include <cstdint>

namespace mbgl {

class TileID;

class SourceInfo {
public:
    std::vector<std::string> tiles;
    uint16_t minZoom = 0;
    uint16_t maxZoom = 22;
    std::string attribution;
    std::array<double, 3> center = { { 0, 0, 0 } };
    std::array<double, 4> bounds = { { -util::LONGITUDE_MAX, -util::LATITUDE_MAX, util::LONGITUDE_MAX, util::LATITUDE_MAX } };
};

} // namespace mbgl

#endif // MBGL_MAP_SOURCE_INFO
