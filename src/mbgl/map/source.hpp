#ifndef MBGL_MAP_SOURCE
#define MBGL_MAP_SOURCE

#include <mbgl/map/tile_id.hpp>
#include <mbgl/map/tile_data.hpp>
#include <mbgl/map/tile_cache.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/ptr.hpp>
#include <mbgl/util/chrono.hpp>

#include <rapidjson/document.h>

#include <cstdint>
#include <forward_list>
#include <iosfwd>
#include <map>

namespace mbgl {

class MapData;
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
class Tile;
struct ClipID;
struct box;

class SourceInfo : private util::noncopyable {
public:
    SourceType type = SourceType::Vector;
    std::string url;
    std::vector<std::string> tiles;
    uint16_t tile_size = 512;
    uint16_t min_zoom = 0;
    uint16_t max_zoom = 22;
    std::string attribution;
    std::array<float, 3> center = {{0, 0, 0}};
    std::array<float, 4> bounds = {{-180, -90, 180, 90}};

    void parseTileJSONProperties(const rapidjson::Value&);
    std::string tileURL(const TileID& id, float pixelRatio) const;
};

class Source : public std::enable_shared_from_this<Source>, private util::noncopyable {
public:
    Source();
    ~Source();

    void load(const std::string& accessToken,
              Environment&,
              std::function<void()> callback);
    bool isLoaded() const;

    void load(MapData&, Environment&, std::function<void()> callback);
    void update(MapData&,
                const TransformState&,
                util::ptr<Style>,
                GlyphAtlas&,
                GlyphStore&,
                SpriteAtlas&,
                util::ptr<Sprite>,
                TexturePool&,
                std::function<void()> callback);

    void invalidateTiles(const std::vector<TileID>&);

    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void drawClippingMasks(Painter &painter);
    void render(Painter &painter, const StyleLayer &layer_desc);
    void finishRender(Painter &painter);

    std::forward_list<Tile *> getLoadedTiles() const;

    void setCacheSize(size_t);
    void onLowMemory();

    SourceInfo info;
    bool enabled;

private:
    bool findLoadedChildren(const TileID& id, int32_t maxCoveringZoom, std::forward_list<TileID>& retain);
    bool findLoadedParent(const TileID& id, int32_t minCoveringZoom, std::forward_list<TileID>& retain);
    int32_t coveringZoomLevel(const TransformState&) const;
    std::forward_list<TileID> coveringTiles(const TransformState&) const;

    TileData::State addTile(MapData&,
                            const TransformState&,
                            util::ptr<Style>,
                            GlyphAtlas&,
                            GlyphStore&,
                            SpriteAtlas&,
                            util::ptr<Sprite>,
                            TexturePool&,
                            const TileID&,
                            std::function<void()> callback);

    TileData::State hasTile(const TileID& id);

    double getZoom(const TransformState &state) const;

    bool loaded = false;

    // Stores the time when this source was most recently updated.
    TimePoint updated = TimePoint::min();

    std::map<TileID, std::unique_ptr<Tile>> tiles;
    std::map<TileID, std::weak_ptr<TileData>> tile_data;
    TileCache cache;
};

}

#endif
