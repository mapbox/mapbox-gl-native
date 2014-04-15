#ifndef LLMR_MAP_SOURCE
#define LLMR_MAP_SOURCE

#include <llmr/map/tile.hpp>
#include <llmr/map/tile_data.hpp>
#include <llmr/util/noncopyable.hpp>

#include <forward_list>
#include <memory>
#include <vector>
#include <string>

namespace llmr {

class Map;
class Transform;
class Painter;
class Texturepool;

class Source : public std::enable_shared_from_this<Source>, private util::noncopyable {
public:
    enum class Type {
        vector,
        raster
    };

public:
    Source(Map &map, Transform &transform, Painter &painter, Texturepool &texturepool,
           const char *url = "", Type type = Type::vector, std::vector<uint32_t> zooms = {{0}},
           uint32_t tile_size = 512, uint32_t min_zoom = 0, uint32_t max_zoom = 14,
           bool enabled = true);

    bool update();
    void prepare_render(bool is_baselayer = false);
    void render(double animationTime, bool is_baselayer = false);

public:
    bool enabled;

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    std::forward_list<Tile::ID> covering_tiles(int32_t zoom, const box& points);

    bool updateTiles();

    TileData::State addTile(const Tile::ID& id);
    TileData::State hasTile(const Tile::ID& id);

    double getZoom() const;

private:
    Map& map;
    Transform& transform;
    Painter& painter;
    Texturepool& texturepool;

    const Type type;
    const std::vector<uint32_t> zooms;
    const std::string url;
    const uint32_t tile_size;
    const uint32_t min_zoom;
    const uint32_t max_zoom;

    std::forward_list<Tile> tiles;
    std::forward_list<std::weak_ptr<TileData>> tile_data;
};

}

#endif
