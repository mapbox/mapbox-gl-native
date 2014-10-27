#ifndef MBGL_MAP_SOURCE
#define MBGL_MAP_SOURCE

#include <mbgl/map/tile.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <forward_list>
#include <iosfwd>
#include <map>

namespace mbgl {

class Map;
class Painter;
class StyleLayer;
class TransformState;
struct box;

class Source : public std::enable_shared_from_this<Source>, private util::noncopyable {
public:
    Source(const util::ptr<SourceInfo>& info);

    void load(Map &map);

    bool update(Map &map);
    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void drawClippingMasks(Painter &painter);
    size_t getTileCount() const;
    void render(Painter &painter, util::ptr<StyleLayer> layer_desc);
    void render(Painter &painter, util::ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix);
    void finishRender(Painter &painter);

    std::forward_list<Tile::ID> getIDs() const;
    std::forward_list<Tile *> getLoadedTiles() const;
    void updateClipIDs(const std::map<Tile::ID, ClipID> &mapping);

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    int32_t coveringZoomLevel(const TransformState&) const;
    std::forward_list<Tile::ID> coveringTiles(const TransformState&) const;

    bool updateTiles(Map &map);

    TileData::State addTile(Map &map, const Tile::ID& id);
    TileData::State hasTile(const Tile::ID& id);

    double getZoom(const TransformState &state) const;

    util::ptr<SourceInfo> info;
    bool loaded = false;

    // Stores the time when this source was most recently updated.
    timestamp updated = 0;

    std::map<Tile::ID, std::unique_ptr<Tile>> tiles;
    std::map<Tile::ID, std::weak_ptr<TileData>> tile_data;
};

}

#endif
