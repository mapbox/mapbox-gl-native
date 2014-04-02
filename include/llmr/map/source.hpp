#ifndef LLMR_MAP_SOURCE
#define LLMR_MAP_SOURCE

#include <llmr/map/tile.hpp>
#include <llmr/map/tile_data.hpp>

#include <forward_list>
#include <memory>
#include <list>
#include <string>

namespace llmr {

class Map;
class Transform;
class Painter;
class Texturepool;
class TileData;

class Source : public std::enable_shared_from_this<Source> {
public:
    enum class Type {
        vector,
        raster
    };

    bool enabled;

public:
    Source(Map& map, Transform& transform, Painter& painter, Texturepool& texturepool, const char *url = "", float pixel_ratio = 1.0, Type type = Type::vector, std::list<uint32_t> zooms = {0}, uint32_t tile_size = 512, uint32_t min_zoom = 0, uint32_t max_zoom = 14, bool enabled = true);

    bool update();
    void render();

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    std::forward_list<Tile::ID> covering_tiles(int32_t zoom, const box& points);

    bool updateTiles();

    TileData::State addTile(const Tile::ID& id);
    TileData::State hasTile(const Tile::ID& id);

    void renderTile(const Tile::ID& id);

    double getZoom() const;

private:
    Map& map;
    Transform& transform;
    Painter& painter;
    Texturepool& texturepool;

    Type type;
    std::list<uint32_t> zooms;
    const char *url;
    uint32_t tile_size;
    uint32_t min_zoom;
    uint32_t max_zoom;

    float pixel_ratio;

    std::forward_list<Tile> tiles;
    std::forward_list<std::weak_ptr<TileData>> tile_data;
};

}

#endif
