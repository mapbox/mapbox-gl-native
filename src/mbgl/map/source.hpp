#ifndef MBGL_MAP_SOURCE
#define MBGL_MAP_SOURCE

#include <mbgl/map/tile.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/style/style_source.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/ptr.hpp>

#include <cstdint>
#include <forward_list>
#include <iosfwd>
#include <map>
#include <chrono>

namespace mbgl {

class Map;
class Environment;
class GlyphAtlas;
class GlyphStore;
class SpriteAtlas;
class Sprite;
class TexturePool;
class Style;
class Painter;
class StyleLayer;
class TransformState;
struct box;

class Source : public std::enable_shared_from_this<Source>, private util::noncopyable {
public:
    Source(SourceInfo&);

    void load(Map &, Environment &);
    void update(Map &, Environment &, uv::worker &, util::ptr<Style>, GlyphAtlas &, GlyphStore &,
                SpriteAtlas &, util::ptr<Sprite>, TexturePool &, std::function<void()> callback);

    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void drawClippingMasks(Painter &painter);
    size_t getTileCount() const;
    void render(Painter &painter, const StyleLayer &layer_desc);
    void render(Painter &painter, const StyleLayer &layer_desc, const Tile::ID &id, const mat4 &matrix);
    void finishRender(Painter &painter);

    std::forward_list<Tile::ID> getIDs() const;
    std::forward_list<Tile *> getLoadedTiles() const;
    void updateClipIDs(const std::map<Tile::ID, ClipID> &mapping);

private:
    bool findLoadedChildren(const Tile::ID& id, int32_t maxCoveringZoom, std::forward_list<Tile::ID>& retain);
    bool findLoadedParent(const Tile::ID& id, int32_t minCoveringZoom, std::forward_list<Tile::ID>& retain);
    int32_t coveringZoomLevel(const TransformState&) const;
    std::forward_list<Tile::ID> coveringTiles(const TransformState&) const;

    TileData::State addTile(Map &, Environment &, uv::worker &, util::ptr<Style>, GlyphAtlas &,
                            GlyphStore &, SpriteAtlas &, util::ptr<Sprite>, TexturePool &,
                            const Tile::ID &, std::function<void()> callback);

    TileData::State hasTile(const Tile::ID& id);

    double getZoom(const TransformState &state) const;

    SourceInfo& info;
    bool loaded = false;

    // Stores the time when this source was most recently updated.
    std::chrono::steady_clock::time_point updated = std::chrono::steady_clock::time_point::min();

    std::map<Tile::ID, std::unique_ptr<Tile>> tiles;
    std::map<Tile::ID, std::weak_ptr<TileData>> tile_data;
};

}

#endif
