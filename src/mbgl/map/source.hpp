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
class Request;
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

class Source : private util::noncopyable {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onSourceLoaded() = 0;
        virtual void onSourceLoadingFailed(std::exception_ptr error) = 0;

        virtual void onTileLoaded(bool isNewTile) = 0;
        virtual void onTileLoadingFailed(std::exception_ptr error) = 0;
    };

    Source();
    ~Source();

    void load();
    bool isLoaded() const;

    void load(MapData&, Environment&, std::function<void()> callback);

    // Request or parse all the tiles relevant for the "TransformState". This method
    // will return true if all the tiles were scheduled for updating of false if
    // they were not. shouldReparsePartialTiles must be set to "true" if there is
    // new data available that a tile in the "partial" state might be interested at.
    bool update(MapData&,
                const TransformState&,
                Style&,
                GlyphAtlas&,
                GlyphStore&,
                SpriteAtlas&,
                util::ptr<Sprite>,
                TexturePool&,
                bool shouldReparsePartialTiles);

    void invalidateTiles(const std::vector<TileID>&);

    void updateMatrices(const mat4 &projMatrix, const TransformState &transform);
    void drawClippingMasks(Painter &painter);
    void finishRender(Painter &painter);

    std::forward_list<Tile *> getLoadedTiles() const;
    const std::vector<Tile*>& getTiles() const;

    void setCacheSize(size_t);
    void onLowMemory();

    void setObserver(Observer* observer);

    SourceInfo info;
    bool enabled;

private:
    void tileLoadingCompleteCallback(const TileID& normalized_id);

    void emitSourceLoaded();
    void emitSourceLoadingFailed(const std::string& message);
    void emitTileLoaded(bool isNewTile);
    void emitTileLoadingFailed(const std::string& message);

    bool handlePartialTile(const TileID &id, Worker &worker);
    bool findLoadedChildren(const TileID& id, int32_t maxCoveringZoom, std::forward_list<TileID>& retain);
    bool findLoadedParent(const TileID& id, int32_t minCoveringZoom, std::forward_list<TileID>& retain);
    int32_t coveringZoomLevel(const TransformState&) const;
    std::forward_list<TileID> coveringTiles(const TransformState&) const;

    TileData::State addTile(MapData&,
                            const TransformState&,
                            Style&,
                            GlyphAtlas&,
                            GlyphStore&,
                            SpriteAtlas&,
                            util::ptr<Sprite>,
                            TexturePool&,
                            const TileID&);

    TileData::State hasTile(const TileID& id);
    void updateTilePtrs();

    double getZoom(const TransformState &state) const;

    bool loaded = false;

    // Stores the time when this source was most recently updated.
    TimePoint updated = TimePoint::min();

    std::map<TileID, std::unique_ptr<Tile>> tiles;
    std::vector<Tile*> tilePtrs;
    std::map<TileID, std::weak_ptr<TileData>> tile_data;
    TileCache cache;

    Request* req = nullptr;
    Observer* observer_ = nullptr;
};

}

#endif
