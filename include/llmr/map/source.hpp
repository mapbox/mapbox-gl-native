#ifndef LLMR_MAP_SOURCE
#define LLMR_MAP_SOURCE

#include <llmr/map/tile.hpp>
#include <llmr/map/tile_data.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/time.hpp>

#include <list>
#include <forward_list>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace llmr {

class Map;
class TransformState;
class Painter;
class Texturepool;

class Source : public std::enable_shared_from_this<Source>, private util::noncopyable {
public:
    enum class Type {
        vector,
        raster
    };

public:
    Source(Map &map, Painter &painter,
           const char *url = "", Type type = Type::vector, uint32_t tile_size = 512,
           uint32_t min_zoom = 0, uint32_t max_zoom = 14, bool enabled = true);

    bool update();
    void updateMatrices(const TransformState &transform);
    void drawClippingMasks();
    size_t getTileCount() const;
    void render(const LayerDescription& layer_desc, const BucketDescription &bucket_desc);
    void finishRender();

    std::forward_list<Tile::ID> getIDs() const;
    void updateClipIDs(const std::map<Tile::ID, ClipID> &mapping);

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
    Painter& painter;

    const Type type;
    const std::string url;
    const uint32_t tile_size;
    const int32_t min_zoom;
    const int32_t max_zoom;

    std::map<Tile::ID, std::unique_ptr<Tile>> tiles;
    std::map<Tile::ID, std::weak_ptr<TileData>> tile_data;
};

}

#endif
