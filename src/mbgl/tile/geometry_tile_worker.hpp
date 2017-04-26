#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/text/glyph.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/optional.hpp>

#include <atomic>
#include <memory>

namespace mbgl {

class GeometryTile;
class GeometryTileData;
class GlyphAtlas;
class SymbolLayout;
class RenderLayer;

namespace style {
class Layer;
} // namespace style

class GeometryTileWorker {
public:
    GeometryTileWorker(ActorRef<GeometryTileWorker> self,
                       ActorRef<GeometryTile> parent,
                       OverscaledTileID,
                       const std::atomic<bool>&,
                       const MapMode);
    ~GeometryTileWorker();

    void setLayers(std::vector<std::unique_ptr<style::Layer>>, uint64_t correlationID);
    void setData(std::unique_ptr<const GeometryTileData>, uint64_t correlationID);
    void setPlacementConfig(PlacementConfig, uint64_t correlationID);
    
    void onGlyphsAvailable(GlyphPositionMap glyphs);
    void onIconsAvailable(IconMap icons);

private:
    void coalesced();
    void redoLayout();
    void attemptPlacement();
    
    void coalesce();

    void requestNewGlyphs(const GlyphDependencies&);
    void requestNewIcons(const IconDependencies&);
   
    void symbolDependenciesChanged();
    bool hasPendingSymbolDependencies() const;
    bool hasPendingSymbolLayouts() const;

    ActorRef<GeometryTileWorker> self;
    ActorRef<GeometryTile> parent;

    const OverscaledTileID id;
    const std::atomic<bool>& obsolete;
    const MapMode mode;

    enum State {
        Idle,
        Coalescing,
        NeedLayout,
        NeedPlacement
    };

    State state = Idle;
    uint64_t correlationID = 0;

    // Outer optional indicates whether we've received it or not.
    optional<std::vector<std::unique_ptr<style::Layer>>> layers;
    optional<std::unique_ptr<const GeometryTileData>> data;
    optional<PlacementConfig> placementConfig;

    std::vector<std::unique_ptr<SymbolLayout>> symbolLayouts;
    GlyphDependencies pendingGlyphDependencies;
    IconDependencies pendingIconDependencies;
    GlyphPositionMap glyphPositions;
    IconMap icons;
};

} // namespace mbgl
