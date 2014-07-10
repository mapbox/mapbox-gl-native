#ifndef LLMR_MAP_SOURCE
#define LLMR_MAP_SOURCE

#include <llmr/map/tile.hpp>
#include <llmr/map/tile_data.hpp>
#include <llmr/util/noncopyable.hpp>
#include <llmr/util/time.hpp>
#include <llmr/style/style_source.hpp>
#include <llmr/style/types.hpp>

#include <list>
#include <forward_list>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <set>

namespace llmr {

class TransformState;
class Texturepool;

class Source : public std::enable_shared_from_this<Source>, private util::noncopyable {
public:
    Source(StyleSource style_source);
    Source(SourceType type = SourceType::Vector, const std::string &url = "",
           uint32_t tile_size = 512, uint32_t min_zoom = 0, uint32_t max_zoom = 22);

    bool update(Map &map);
    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void drawClippingMasks(Painter &painter);
    size_t getTileCount() const;
    void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc);
    void finishRender(Painter &painter);

    std::forward_list<Tile::ID> getIDs() const;
    void updateClipIDs(const std::map<Tile::ID, ClipID> &mapping);

    static std::string normalizeSourceURL(const std::string &url);

public:
    const SourceType type;
    const std::string url;
    const uint32_t tile_size;
    const int32_t min_zoom;
    const int32_t max_zoom;

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    std::forward_list<Tile::ID> covering_tiles(const TransformState &state, int32_t clamped_zoom, const box& points);

    bool updateTiles(Map &map);

    TileData::State addTile(Map &map, const Tile::ID& id);
    TileData::State hasTile(const Tile::ID& id);

    double getZoom(const TransformState &state) const;

private:
    // Stores the time when this source was most recently updated.
    timestamp updated = 0;

    std::map<Tile::ID, std::unique_ptr<Tile>> tiles;
    std::map<Tile::ID, std::weak_ptr<TileData>> tile_data;
};

}

#endif
