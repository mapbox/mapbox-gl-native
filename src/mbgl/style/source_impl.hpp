#pragma once

#include <mbgl/style/source.hpp>

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/range.hpp>

#include <memory>
#include <unordered_map>
#include <vector>
#include <map>

namespace mbgl {

class Painter;
class FileSource;
class TransformState;
class RenderTile;

namespace algorithm {
class ClipIDGenerator;
} // namespace algorithm

namespace style {

class UpdateParameters;
class QueryParameters;
class SourceObserver;

class Source::Impl : public TileObserver, private util::noncopyable {
public:
    Impl(SourceType, std::string id, Source&);
    ~Impl() override;

    virtual void loadDescription(FileSource&) = 0;
    bool isLoaded() const;

    // Request or parse all the tiles relevant for the "TransformState". This method
    // will return true if all the tiles were scheduled for updating of false if
    // they were not. shouldReparsePartialTiles must be set to "true" if there is
    // new data available that a tile in the "partial" state might be interested at.
    void loadTiles(const UpdateParameters&);
    bool parseTiles(const UpdateParameters&);

    // Request that all loaded tiles re-run the layout operation on the existing source
    // data with fresh style information.
    void reload();

    void startRender(algorithm::ClipIDGenerator&, const mat4& projMatrix, const TransformState&);
    void finishRender(Painter&);

    const std::map<UnwrappedTileID, RenderTile>& getRenderTiles() const;

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const QueryParameters&) const;

    void setCacheSize(size_t);
    void onLowMemory();

    void setObserver(SourceObserver*);
    void dumpDebugLogs() const;

    const SourceType type;
    const std::string id;

    bool loaded = false;

    // Tracks whether the source is used by any layers visible at the current zoom level. Must
    // be initialized to true so that Style::isLoaded() does not produce false positives if
    // called before Style::recalculate().
    bool enabled = true;

protected:
    void invalidateTiles();

    Source& base;
    SourceObserver* observer = nullptr;

private:
    // TileObserver implementation.
    void onTileLoaded(Tile&, TileLoadState) override;
    void onTileError(Tile&, std::exception_ptr) override;
    void onTileUpdated(Tile&) override;

    virtual uint16_t getTileSize() const = 0;
    virtual Range<uint8_t> getZoomRange() = 0;
    virtual std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) = 0;

    std::map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    std::map<UnwrappedTileID, RenderTile> renderTiles;
    TileCache cache;
};

} // namespace style
} // namespace mbgl
