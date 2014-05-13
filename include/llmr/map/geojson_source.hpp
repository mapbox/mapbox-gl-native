#ifndef LLMR_MAP_GEOJSON_SOURCE
#define LLMR_MAP_GEOJSON_SOURCE

#include <llmr/map/source.hpp>

#include <memory>

#include <rapidjson/document.h>

namespace llmr {

using JSVal = const rapidjson::Value&;
using Segment = std::vector<Coordinate>;
using GeoJSONTile = std::vector<Segment>;

class Map;
class Transform;

class GeoJSONSource : public Source, private util::noncopyable {
public:
    GeoJSONSource(Map &map, Painter &painter, const char *url = "",
                  std::vector<uint32_t> zooms = {{ 1, 5, 9, 13 }}, uint32_t tile_size = 512,
                  uint32_t min_zoom = 1, uint32_t max_zoom = 13, bool enabled = true);

private:
    rapidjson::Document parseJSON(const char* data);
    void tile_GeoJSON(JSVal geojson);
    void tile_feature(JSVal feature, std::shared_ptr<Transform>& transform);
    std::map<Tile::ID, GeoJSONTile> tile_line_string(JSVal coords, std::shared_ptr<Transform>& transform, bool rejoin = false);

private:
    std::map<Tile::ID, GeoJSONTile> all_tiles;
    uint16_t tile_extent = 4096;
    double padding = 0.01;
    std::vector<std::shared_ptr<Transform>> transforms;

};

}

#endif
