#ifndef MBGL_MAP_SOURCE
#define MBGL_MAP_SOURCE

#include <mbgl/map/tile.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/mat4.hpp>

#include <cstdint>
#include <forward_list>
#include <iosfwd>
#include <map>
#include <memory>

namespace mbgl {

class Map;
class Painter;
class StyleLayer;
class TransformState;
struct box;

class Source : public std::enable_shared_from_this<Source>, private util::noncopyable {
public:
    Source(SourceInfo info, const std::string &access_token = "");

    bool update(Map &map);
    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void drawClippingMasks(Painter &painter);
    size_t getTileCount() const;
    void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc);
    void render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID &id, const mat4 &matrix);
    void finishRender(Painter &painter);

    std::forward_list<Tile::ID> getIDs() const;
    void updateClipIDs(const std::map<Tile::ID, ClipID> &mapping);

    static std::string normalizeSourceURL(const std::string &url, const std::string &access_token);

public:
    const SourceInfo info;

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
