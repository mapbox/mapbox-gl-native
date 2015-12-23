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
    SourceType type = SourceType::Vector;
    std::string url;
    std::vector<std::string> tiles;
    uint16_t tile_size = util::tileSize;
    uint16_t min_zoom = 0;
    uint16_t max_zoom = 22;
    std::string attribution;
    std::array<float, 3> center = { { 0, 0, 0 } };
    std::array<float, 4> bounds = { { -180, -90, 180, 90 } };
    std::string source_id = "";

    std::string tileURL(const TileID&, float pixelRatio) const;
};

} // namespace mbgl

#endif // MBGL_MAP_SOURCE_INFO
