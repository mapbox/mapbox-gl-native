#ifndef MBGL_MAP_SOURCE_INFO
#define MBGL_MAP_SOURCE_INFO

#include <mbgl/style/types.hpp>
#include <mbgl/map/tile_id.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/constants.hpp>

#include <mapbox/variant.hpp>
#include <rapidjson/document.h>

#include <array>
#include <vector>
#include <cstdint>

namespace mapbox {
namespace geojsonvt {
class GeoJSONVT;
} // namespace geojsonvt
} // namespace mapbox

namespace mbgl {

class SourceInfo : private util::noncopyable {
public:
    ~SourceInfo();

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
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> geojsonvt;

    void parseTileJSONProperties(const rapidjson::Value&);
    void parseGeoJSON(const rapidjson::Value&);
    std::string tileURL(const TileID& id, float pixelRatio) const;
};

} // namespace mbgl

#endif // MBGL_MAP_SOURCE_INFO
