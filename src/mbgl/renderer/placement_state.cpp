#include <mbgl/renderer/placement_state.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>


namespace mbgl {

LayerPlacementState::LayerPlacementState() : currentTileIndex(0)
{
}

bool LayerPlacementState::continuePlacement(RenderSymbolLayer& layer,
                                       CollisionIndex& collisionIndex,
                                       const bool showCollisionBoxes,
                                       std::function<bool()> shouldPausePlacement) {
    while (currentTileIndex < layer.renderTiles.size()) {
        RenderTile& tile = layer.renderTiles[currentTileIndex++];
        tile.tile.placeLayer(showCollisionBoxes, collisionIndex, layer.impl());

        if (shouldPausePlacement()) {
            return true;
        }
    }
    return false;
}

PlacementState::PlacementState(const TransformState& transformState,
                               const std::vector<RenderLayer>& order,
                               const bool forceFullPlacement_,
                               const bool showCollisionBoxes_,
                               Duration fadeDuration,
                               optional<PlacementState>& previousPlacement)
   : collisionIndex(transformState)
   , currentLayerIndex(order.size() - 1)
   , forceFullPlacement(forceFullPlacement_)
   , showCollisionBoxes(showCollisionBoxes_)
{
    if (forceFullPlacement || !previousPlacement) {
        delayUntil = TimePoint();
    } else {
        delayUntil = previousPlacement->delayUntil + Milliseconds(300);
    }

    if (previousPlacement) {
        collisionFadeTimes = previousPlacement->collisionFadeTimes;
    } else {
        collisionFadeTimes = { TimePoint::min(), fadeDuration };
    }
}

void PlacementState::continuePlacement(std::vector<RenderLayer>& order, std::vector<RenderSource>& sources) {
    const TimePoint startTime;

    if (startTime < delayUntil) return;

    auto shouldPausePlacement = [&] () {
        const Duration elapsedTime = TimePoint() - startTime;
        return forceFullPlacement ? false : elapsedTime > Milliseconds(2);
    };

    do {
        RenderLayer& renderLayer = order[currentLayerIndex];
        if (renderLayer.is<RenderSymbolLayer>()) {
            if (inProgressLayer) {
                inProgressLayer = LayerPlacementState();
            }

            bool pausePlacement = inProgressLayer->continuePlacement(*renderLayer.as<RenderSymbolLayer>(),
                                                                     collisionIndex,
                                                                     showCollisionBoxes,
                                                                     shouldPausePlacement);

            if (pausePlacement) {
                // We didn't finish placing all layers within 2ms,
                // but we can keep rendering with a partial placement
                // We'll resume here on the next frame
                return;
            }

            inProgressLayer = std::experimental::nullopt;
        }

        currentLayerIndex--;
    } while (currentLayerIndex > 0);

    for (RenderSource& source : sources) {
        auto tiles = source.getRenderTiles();
        for (RenderTile& tile : tiles) {
            tile.tile.commitPlacement(collisionIndex, collisionFadeTimes);
        }
    }

    done = true;
}

bool PlacementState::stillFading() const {
    return TimePoint() < collisionFadeTimes.latestStart + collisionFadeTimes.fadeDuration;
}

}; // mbgl
