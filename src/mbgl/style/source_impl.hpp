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

    // Called when the camera has changed. May load new tiles, unload obsolete tiles, or
    // trigger re-placement of existing complete tiles.
    void updateTiles(const UpdateParameters&);

    // Called when icons or glyphs are loaded. Triggers further processing of tiles which
    // were waiting on such dependencies.
    void updateSymbolDependentTiles();

    // Removes all tiles (by putting them into the cache).
    void removeTiles();

    // Request that all loaded tiles re-run the layout operation on the existing source
    // data with fresh style information.
    void reloadTiles();

    void startRender(algorithm::ClipIDGenerator&,
                     const mat4& projMatrix,
                     const TransformState&);
    void finishRender(Painter&);

    std::map<UnwrappedTileID, RenderTile>& getRenderTiles();

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const QueryParameters&) const;

    void setCacheSize(size_t);
    void onLowMemory();

    void setObserver(SourceObserver*);
    void dumpDebugLogs() const;

    const SourceType type;
    const std::string id;

    virtual optional<std::string> getAttribution() const { return {}; };

    bool loaded = false;

    // Tracks whether the source is used by any layers visible at the current zoom level. Must
    // be initialized to true so that Style::isLoaded() does not produce false positives if
    // called before Style::recalculate().
    bool enabled = true;

protected:
    void invalidateTiles();
    void removeStaleTiles(const std::set<OverscaledTileID>&);

    Source& base;
    SourceObserver* observer = nullptr;
    std::map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    TileCache cache;

private:
    // TileObserver implementation.
    void onTileChanged(Tile&) override;
    void onTileError(Tile&, std::exception_ptr) override;

    virtual uint16_t getTileSize() const = 0;
    virtual Range<uint8_t> getZoomRange() = 0;
    virtual std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) = 0;

    std::map<UnwrappedTileID, RenderTile> renderTiles;
};

} // namespace style
} // namespace mbgl
