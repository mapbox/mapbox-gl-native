#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/text/placement_config.hpp>
#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/util/optional.hpp>

#include <atomic>
#include <memory>
#include <unordered_map>

namespace mbgl {

class GeometryTile;
class GeometryTileData;
class GlyphAtlas;
class SymbolLayout;

namespace style {
class Layer;
} // namespace style

class GeometryTileWorker {
public:
    GeometryTileWorker(ActorRef<GeometryTileWorker> self,
                       ActorRef<GeometryTile> parent,
                       OverscaledTileID,
                       GlyphAtlas&,
                       const std::atomic<bool>&,
                       const MapMode);
    ~GeometryTileWorker();

    void setLayers(std::vector<std::unique_ptr<style::Layer>>, uint64_t correlationID);
    void setData(std::unique_ptr<const GeometryTileData>, uint64_t correlationID);
    void setPlacementConfig(PlacementConfig, uint64_t correlationID);
    void symbolDependenciesChanged();

private:
    void coalesce();
    void coalesced();
    void redoLayout();
    void attemptPlacement();
    bool hasPendingSymbolDependencies() const;

    ActorRef<GeometryTileWorker> self;
    ActorRef<GeometryTile> parent;

    const OverscaledTileID id;
    GlyphAtlas& glyphAtlas;
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
};

} // namespace mbgl
