#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_observer.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_cache.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layer_impl.hpp>

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
class RenderTile;
class RenderLayer;
class RenderedQueryOptions;
class SourceQueryOptions;
class TileParameters;

class TilePyramid {
public:
    TilePyramid();
    ~TilePyramid();

    bool isLoaded() const;

    void update(const std::vector<Immutable<style::Layer::Impl>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&,
                style::SourceType type,
                uint16_t tileSize,
                Range<uint8_t> zoomRange,
                optional<LatLngBounds> bounds,
                std::function<std::unique_ptr<Tile> (const OverscaledTileID&)> createTile);

    void startRender(PaintParameters&);
    void finishRender(PaintParameters&);

    std::vector<std::reference_wrapper<RenderTile>> getRenderTiles();
    Tile* getTile(const OverscaledTileID&);

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::vector<const RenderLayer*>&,
                          const RenderedQueryOptions& options,
                          const CollisionIndex& collisionIndex) const;

    std::vector<Feature> querySourceFeatures(const SourceQueryOptions&) const;

    void setCacheSize(size_t);
    void reduceMemoryUse();

    void setObserver(TileObserver*);
    void dumpDebugLogs() const;

    bool enabled = false;

    std::map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    TileCache cache;

    std::vector<RenderTile> renderTiles;

    TileObserver* observer = nullptr;
};

} // namespace mbgl
