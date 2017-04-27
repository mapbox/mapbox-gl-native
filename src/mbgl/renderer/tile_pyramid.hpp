#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/style/types.hpp>

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/range.hpp>

#include <memory>
#include <unordered_map>
#include <vector>
#include <map>

namespace mbgl {

class Painter;
class TransformState;
class RenderTile;
class RenderedQueryOptions;
class SourceQueryOptions;
class TileParameters;

class TilePyramid {
public:
    TilePyramid();
    ~TilePyramid();

    bool isLoaded() const;

    // Called when the camera has changed. May load new tiles, unload obsolete tiles, or
    // trigger re-placement of existing complete tiles.
    void updateTiles(const TileParameters&,
                     SourceType type,
                     uint16_t tileSize,
                     Range<uint8_t> zoomRange,
                     std::function<std::unique_ptr<Tile> (const OverscaledTileID&)> createTile);

    // Removes all tiles (by putting them into the cache).
    void removeTiles();

    // Remove all tiles and clear the cache.
    void invalidateTiles();

    // Request that all loaded tiles re-run the layout operation on the existing source
    // data with fresh style information.
    void reloadTiles();

    void startRender(const mat4& projMatrix,
                     const mat4& clipMatrix,
                     const TransformState&);
    void finishRender(Painter&);

    std::map<UnwrappedTileID, RenderTile>& getRenderTiles();

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const RenderedQueryOptions& options) const;

    std::vector<Feature> querySourceFeatures(const SourceQueryOptions&) const;

    void setCacheSize(size_t);
    void onLowMemory();

    void setObserver(TileObserver*);
    void dumpDebugLogs() const;

    bool enabled = false;

    void removeStaleTiles(const std::set<OverscaledTileID>&);

    std::map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    TileCache cache;

    std::map<UnwrappedTileID, RenderTile> renderTiles;

    TileObserver* observer = nullptr;
};

} // namespace mbgl
