#include <mbgl/text/placement.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/geometry_tile.cpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

OpacityState::OpacityState(bool placed_) : opacity(0), placed(placed_) {}

OpacityState::OpacityState(const OpacityState& prevState, float increment, bool placed_) :
    opacity(std::fmax(0, std::fmin(1, prevState.opacity + (prevState.placed ? increment : -increment)))),
    placed(placed_) {}

bool OpacityState::isHidden() const {
    return opacity == 0 && !placed;
}

JointOpacityState::JointOpacityState(bool placedIcon, bool placedText) :
    icon(OpacityState(placedIcon)),
    text(OpacityState(placedText)) {}

JointOpacityState::JointOpacityState(const JointOpacityState& prevOpacityState, float increment, bool placedIcon, bool placedText) :
    icon(OpacityState(prevOpacityState.icon, increment, placedIcon)),
    text(OpacityState(prevOpacityState.text, increment, placedText)) {}

bool JointOpacityState::isHidden() const {
    return icon.isHidden() && text.isHidden();
}

Placement::Placement(const TransformState& state_, MapMode mapMode_, const mat4& projMatrix_, const bool showCollisionBoxes_, Scheduler& scheduler)
    : result(makeMutable<PlacementResult>(state_))
    , state(state_)
    , mapMode(mapMode_)
    , showCollisionBoxes(showCollisionBoxes_)
    , projMatrix(projMatrix_)
    , worker(scheduler, state_, projMatrix_, showCollisionBoxes_)
    , placementBucketLayers(makeMutable<std::vector<std::vector<PlacementBucket>>>())
    , recentUntil(TimePoint::min())
{}

void Placement::addLayer(RenderSymbolLayer& symbolLayer) {

    placementBucketLayers->emplace_back();
    auto& placementBuckets = placementBucketLayers->back();

    for (RenderTile& renderTile : symbolLayer.renderTiles) {

        if (!renderTile.tile.isRenderable()) {
            continue;
        }

        auto bucket = renderTile.tile.getBucket(*symbolLayer.baseImpl);
        assert(dynamic_cast<SymbolBucket*>(bucket));
        SymbolBucket& symbolBucket = *reinterpret_cast<SymbolBucket*>(bucket);

        assert(symbolBucket.core);
        placementBuckets.emplace_back(*symbolBucket.core, *symbolBucket.symbolCrossTileIDs, renderTile.id, renderTile.tile.id);
    }
}

void Placement::place() {
    if (mapMode == MapMode::Still) {
        // synchronous
        PlacementWorker placement(state, projMatrix, showCollisionBoxes);
        result = placement.place(std::move(placementBucketLayers));
    } else {
        // asynchronous
        future = worker.ask(&PlacementWorker::place, std::move(placementBucketLayers));
    }
}

bool Placement::isReady() {
    if (mapMode == MapMode::Still) {
        // synchronous
        return true;
    } else {
        // asynchronous
        return future.wait_for(Duration(0)) == std::future_status::ready;
    }
}

bool Placement::commit(const Placement& prevPlacement, TimePoint now) {

    if (mapMode != MapMode::Still) {
        // asynchronous result
        result = future.get();
    }

    commitTime = now;

    bool placementChanged = false;

    float increment = mapMode == MapMode::Still ? 1.0 : std::chrono::duration<float>(commitTime - prevPlacement.commitTime) / Duration(std::chrono::milliseconds(300));

    auto& placements = result->placements;

    if (increment) {}
    // add the opacities from the current placement, and copy their current values from the previous placement
    for (auto& placementPair : placements) {
        auto prevOpacity = prevPlacement.opacities.find(placementPair.first);
        if (prevOpacity != prevPlacement.opacities.end()) {
            opacities.emplace(placementPair.first, JointOpacityState(prevOpacity->second, increment, placementPair.second.icon, placementPair.second.text));
            placementChanged = placementChanged ||
                placementPair.second.icon != prevOpacity->second.icon.placed ||
                placementPair.second.text != prevOpacity->second.text.placed;
        } else {
            opacities.emplace(placementPair.first, JointOpacityState(placementPair.second.icon, placementPair.second.text));
            placementChanged = placementChanged || placementPair.second.icon || placementPair.second.text;
        }
    }

    // copy and update values from the previous placement that aren't in the current placement but haven't finished fading
    for (auto& prevOpacity : prevPlacement.opacities) {
        if (opacities.find(prevOpacity.first) == opacities.end()) {
            JointOpacityState jointOpacity(prevOpacity.second, increment, false, false);
            if (!jointOpacity.isHidden()) {
                opacities.emplace(prevOpacity.first, jointOpacity);
                placementChanged = placementChanged || prevOpacity.second.icon.placed || prevOpacity.second.text.placed;
            }
        }
    }

    return placementChanged;
}

void Placement::updateLayerOpacities(RenderSymbolLayer& symbolLayer) {
    std::unordered_set<uint32_t> seenCrossTileIDs;
    for (RenderTile& renderTile : symbolLayer.renderTiles) {
        if (!renderTile.tile.isRenderable()) {
            continue;
        }

        auto bucket = renderTile.tile.getBucket(*symbolLayer.baseImpl);
        assert(dynamic_cast<SymbolBucket*>(bucket));
        SymbolBucket& symbolBucket = *reinterpret_cast<SymbolBucket*>(bucket);
        updateBucketOpacities(symbolBucket, seenCrossTileIDs);
    }
}

void Placement::updateBucketOpacities(SymbolBucket& bucket, std::unordered_set<uint32_t>& seenCrossTileIDs) {
    if (bucket.hasTextData()) {
        bucket.text.opacityVertices.clear();
        bucket.text.placedSymbolVisibility.clear();
    }
    if (bucket.hasIconData()) {
        bucket.icon.opacityVertices.clear();
        bucket.icon.placedSymbolVisibility.clear();
    }
    if (bucket.hasCollisionBoxData()) bucket.collisionBox.dynamicVertices.clear();
    if (bucket.hasCollisionCircleData()) bucket.collisionCircle.dynamicVertices.clear();

    auto& symbolInstances = (*bucket.core)->symbolInstances;

    auto idIt = (*bucket.symbolCrossTileIDs)->begin();
    for (auto it = symbolInstances.begin(); it != symbolInstances.end(); it++, idIt++) {
        auto& symbolInstance = *it;
        auto crossTileID = *idIt;

        auto opacityState = seenCrossTileIDs.count(crossTileID) == 0 ?
            getOpacity(crossTileID) :
            JointOpacityState(false, false);

        seenCrossTileIDs.insert(crossTileID);

        if (symbolInstance.hasText) {
            auto opacityVertex = SymbolOpacityAttributes::vertex(opacityState.text.placed, opacityState.text.opacity);
            for (size_t i = 0; i < symbolInstance.horizontalGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            for (size_t i = 0; i < symbolInstance.verticalGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            if (symbolInstance.placedTextIndex) {
                bucket.text.placedSymbolVisibility.push_back(opacityState.isHidden());
            }
            if (symbolInstance.placedVerticalTextIndex) {
                bucket.text.placedSymbolVisibility.push_back(opacityState.isHidden());
            }
        }
        if (symbolInstance.hasIcon) {
            auto opacityVertex = SymbolOpacityAttributes::vertex(opacityState.icon.placed, opacityState.icon.opacity);
            if (symbolInstance.iconQuad) {
                bucket.icon.opacityVertices.emplace_back(opacityVertex);
                bucket.icon.opacityVertices.emplace_back(opacityVertex);
                bucket.icon.opacityVertices.emplace_back(opacityVertex);
                bucket.icon.opacityVertices.emplace_back(opacityVertex);
            }
            if (symbolInstance.placedIconIndex) {
                bucket.icon.placedSymbolVisibility.push_back(opacityState.isHidden());
            }
        }
        
        auto updateCollisionBox = [&](const auto& feature, const bool placed) {
            for (const CollisionBox& box : feature.boxes) {
                if (feature.alongLine) {
                   auto dynamicVertex = CollisionBoxDynamicAttributes::vertex(placed, !box.used);
                    bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
                    bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
                    bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
                    bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
                } else {
                    auto dynamicVertex = CollisionBoxDynamicAttributes::vertex(placed, false);
                    bucket.collisionBox.dynamicVertices.emplace_back(dynamicVertex);
                    bucket.collisionBox.dynamicVertices.emplace_back(dynamicVertex);
                    bucket.collisionBox.dynamicVertices.emplace_back(dynamicVertex);
                    bucket.collisionBox.dynamicVertices.emplace_back(dynamicVertex);
                }
            }
        };
        updateCollisionBox(symbolInstance.textCollisionFeature, opacityState.text.placed);
        updateCollisionBox(symbolInstance.iconCollisionFeature, opacityState.icon.placed);
    }

    bucket.updateOpacity();
    bucket.sortFeatures(state.getAngle());
}

JointOpacityState Placement::getOpacity(uint32_t crossTileSymbolID) const {
    auto it = opacities.find(crossTileSymbolID);
    if (it != opacities.end()) {
        return it->second;
    } else {
        return JointOpacityState(false, false);
    }

}

float Placement::symbolFadeChange(TimePoint now) const {
    if (mapMode == MapMode::Still) {
        return 1.0;
    }
    return std::chrono::duration<float>(now - commitTime) / Duration(std::chrono::milliseconds(300));
}

bool Placement::hasTransitions(TimePoint now) const {
    return symbolFadeChange(now) < 1.0 || stale;
}

bool Placement::stillRecent(TimePoint now) const {
    return mapMode == MapMode::Continuous && recentUntil > now;
}
void Placement::setRecent(TimePoint now) {
    stale = false;
    if (mapMode == MapMode::Continuous) {
        // Only set in continuous mode because "now" isn't defined in still mode
        recentUntil = now + Duration(std::chrono::milliseconds(300));
    }
}

void Placement::setStale() {
    stale = true;
}



} // namespace mbgl
