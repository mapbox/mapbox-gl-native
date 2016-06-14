#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/range.hpp>

#include <vector>
#include <map>

namespace mbgl {

class Painter;
class FileSource;
class TransformState;
class RenderTile;
struct ClipID;

namespace algorithm {
class ClipIDGenerator;
} // namespace algorithm

namespace style {

class Style;
class UpdateParameters;
class QueryParameters;
class SourceObserver;

class Source : public TileObserver, private util::noncopyable {
public:
    Source(SourceType, std::string id);
    ~Source() override;

    bool loaded = false;
    virtual void load(FileSource&) = 0;
    bool isLoaded() const;

    // Request or parse all the tiles relevant for the "TransformState". This method
    // will return true if all the tiles were scheduled for updating of false if
    // they were not. shouldReparsePartialTiles must be set to "true" if there is
    // new data available that a tile in the "partial" state might be interested at.
    bool update(const UpdateParameters&);

    void startRender(algorithm::ClipIDGenerator&,
                     const mat4& projMatrix,
                     const TransformState&);
    void finishRender(Painter&);

    const std::map<UnwrappedTileID, RenderTile>& getRenderTiles() const;

    Tile* getTile(const OverscaledTileID&) const;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const QueryParameters&) const;

    void setCacheSize(size_t);
    void onLowMemory();

    void setObserver(SourceObserver* observer);
    void dumpDebugLogs() const;

    const SourceType type;
    const std::string id;
    bool enabled = false;

private:
    // TileObserver implementation.
    void onTileLoaded(Tile&, bool isNewTile) override;
    void onTileError(Tile&, std::exception_ptr) override;
    void onNeedsRepaint() override;

    virtual uint16_t getTileSize() const = 0;
    virtual Range<uint8_t> getZoomRange() = 0;
    virtual std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) = 0;

protected:
    void invalidateTiles();

    SourceObserver* observer = nullptr;

private:
    // Stores the time when this source was most recently updated.
    TimePoint updated = TimePoint::min();

    std::map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    std::map<UnwrappedTileID, RenderTile> renderTiles;
    TileCache cache;
};

} // namespace style
} // namespace mbgl
