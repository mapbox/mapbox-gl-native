#pragma once

#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/collection.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/optional.hpp>

#include <functional>
#include <map>
#include <string>

namespace mbgl {

// TODO: Placeholder
class CollisionIndex {
public:
    CollisionIndex(const TransformState&) {}
};

class RenderLayer;
class RenderSymbolLayer;
class RenderSource;

struct CollisionFadeTimes {
    TimePoint latestStart;
    Duration fadeDuration;
};

class LayerPlacementState {
public:
    LayerPlacementState();
    bool continuePlacement(RenderSymbolLayer& renderLayer,
                           CollisionIndex& collisionIndex,
                           const bool showCollisionBoxes,
                           std::function<bool()> shouldPausePlacement);

private:
    size_t currentTileIndex;
};

class PlacementState
{
public:
    PlacementState(const TransformState& transformState,
                   const std::vector<RenderLayer>& order,
                   const bool forceFullPlacement,
                   const bool showCollisionBoxes,
                   Duration fadeDuration,
                   optional<PlacementState>& previousPlacement);
    
    bool isDone() const { return done; }
    bool stillFading() const;
    
    void continuePlacement(std::vector<RenderLayer>& order, std::vector<RenderSource>& sources);
    
private:
    CollisionIndex collisionIndex;
    bool done;
    size_t currentLayerIndex;
    bool forceFullPlacement;
    bool showCollisionBoxes;
    TimePoint delayUntil;
    CollisionFadeTimes collisionFadeTimes;
    optional<LayerPlacementState> inProgressLayer;
    
};

}; // mbgl
