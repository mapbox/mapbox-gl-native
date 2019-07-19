#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_properties.hpp>

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/range.hpp>

#include <memory>
#include <unordered_map>
#include <vector>
#include <map>

namespace mbgl {

class PaintParameters;
class TransformState;
class RenderLayer;
class RenderedQueryOptions;
class SourceQueryOptions;
class TileParameters;
class SourcePrepareParameters;

class TilePyramid {
public:
    TilePyramid();
    ~TilePyramid();

    bool isLoaded() const;

    void update(const std::vector<Immutable<style::LayerProperties>>& visibleLayers,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&,
                style::SourceType type,
                uint16_t tileSize,
                Range<uint8_t> zoomRange,
                optional<LatLngBounds> bounds,
                std::function<std::unique_ptr<Tile> (const OverscaledTileID&)> createTile);

    const std::map<UnwrappedTileID, std::reference_wrapper<Tile>>& getRenderedTiles() const { return renderedTiles; }
    Tile* getTile(const OverscaledTileID&);
    const Tile* getRenderedTile(const UnwrappedTileID&) const;

    void handleWrapJump(float lng);

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::unordered_map<std::string, const RenderLayer*>&,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const;

    std::vector<Feature> querySourceFeatures(const SourceQueryOptions&) const;

    void setCacheSize(size_t);
    void reduceMemoryUse();

    void setObserver(TileObserver*);
    void dumpDebugLogs() const;

    const std::map<OverscaledTileID, std::unique_ptr<Tile>>& getTiles() const { return tiles; }
    void clearAll();

    void updateFadingTiles();
    bool hasFadingTiles() const { return fadingTiles; }

private:
    void addRenderTile(const UnwrappedTileID& tileID, Tile& tile);

    std::map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    TileCache cache;

    std::map<UnwrappedTileID, std::reference_wrapper<Tile>> renderedTiles; // Sorted by tile id.
    TileObserver* observer = nullptr;

    float prevLng = 0;

    bool fadingTiles = false;
};

} // namespace mbgl
