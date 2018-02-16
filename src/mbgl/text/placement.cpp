#include <mbgl/text/placement.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

OpacityState::OpacityState(bool placed_, bool skipFade)
    : opacity((skipFade && placed_) ? 1 : 0)
    , placed(placed_)
{
}

OpacityState::OpacityState(const OpacityState& prevState, float increment, bool placed_) :
    opacity(::fmax(0, ::fmin(1, prevState.opacity + (prevState.placed ? increment : -increment)))),
    placed(placed_) {}

bool OpacityState::isHidden() const {
    return opacity == 0 && !placed;
}

JointOpacityState::JointOpacityState(bool placedText, bool placedIcon, bool skipFade) :
    icon(OpacityState(placedIcon, skipFade)),
    text(OpacityState(placedText, skipFade)) {}

JointOpacityState::JointOpacityState(const JointOpacityState& prevOpacityState, float increment, bool placedText, bool placedIcon) :
    icon(OpacityState(prevOpacityState.icon, increment, placedIcon)),
    text(OpacityState(prevOpacityState.text, increment, placedText)) {}

bool JointOpacityState::isHidden() const {
    return icon.isHidden() && text.isHidden();
}

Placement::Placement(const TransformState& state_, MapMode mapMode_)
    : collisionIndex(state_)
    , state(state_)
    , mapMode(mapMode_)
    , recentUntil(TimePoint::min())
{}

void Placement::placeLayer(RenderSymbolLayer& symbolLayer, const mat4& projMatrix, bool showCollisionBoxes) {

    std::unordered_set<uint32_t> seenCrossTileIDs;

    for (RenderTile& renderTile : symbolLayer.renderTiles) {
        if (!renderTile.tile.isRenderable()) {
            continue;
        }

        auto bucket = renderTile.tile.getBucket(*symbolLayer.baseImpl);
        assert(dynamic_cast<SymbolBucket*>(bucket));
        SymbolBucket& symbolBucket = *reinterpret_cast<SymbolBucket*>(bucket);

        auto& layout = symbolBucket.layout;

        const float pixelsToTileUnits = renderTile.id.pixelsToTileUnits(1, state.getZoom());

        const float scale = std::pow(2, state.getZoom() - renderTile.tile.id.overscaledZ);
        const float textPixelRatio = (util::tileSize * renderTile.tile.id.overscaleFactor()) / util::EXTENT;

        mat4 posMatrix;
        state.matrixFor(posMatrix, renderTile.id);
        matrix::multiply(posMatrix, projMatrix, posMatrix);

        mat4 textLabelPlaneMatrix = getLabelPlaneMatrix(posMatrix,
                layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map,
                layout.get<style::TextRotationAlignment>() == style::AlignmentType::Map,
                state,
                pixelsToTileUnits);

        mat4 iconLabelPlaneMatrix = getLabelPlaneMatrix(posMatrix,
                layout.get<style::IconPitchAlignment>() == style::AlignmentType::Map,
                layout.get<style::IconRotationAlignment>() == style::AlignmentType::Map,
                state,
                pixelsToTileUnits);

        placeLayerBucket(symbolBucket, posMatrix, textLabelPlaneMatrix, iconLabelPlaneMatrix, scale, textPixelRatio, showCollisionBoxes, seenCrossTileIDs, renderTile.tile.holdForFade());
    }
}

void Placement::placeLayerBucket(
        SymbolBucket& bucket,
        const mat4& posMatrix,
        const mat4& textLabelPlaneMatrix,
        const mat4& iconLabelPlaneMatrix,
        const float scale,
        const float textPixelRatio,
        const bool showCollisionBoxes,
        std::unordered_set<uint32_t>& seenCrossTileIDs,
        const bool holdingForFade) {

    auto partiallyEvaluatedTextSize = bucket.textSizeBinder->evaluateForZoom(state.getZoom());
    auto partiallyEvaluatedIconSize = bucket.iconSizeBinder->evaluateForZoom(state.getZoom());

    const bool iconWithoutText = !bucket.hasTextData() || bucket.layout.get<style::TextOptional>();
    const bool textWithoutIcon = !bucket.hasIconData() || bucket.layout.get<style::IconOptional>();

    for (auto& symbolInstance : bucket.symbolInstances) {

        if (seenCrossTileIDs.count(symbolInstance.crossTileID) == 0) {
            if (holdingForFade) {
                // Mark all symbols from this tile as "not placed", but don't add to seenCrossTileIDs, because we don't
                // know yet if we have a duplicate in a parent tile that _should_ be placed.
                placements.emplace(symbolInstance.crossTileID, JointPlacement(false, false, false));
                continue;
            }

            bool placeText = false;
            bool placeIcon = false;
            bool offscreen = true;

            if (symbolInstance.placedTextIndex) {
                PlacedSymbol& placedSymbol = bucket.text.placedSymbols.at(*symbolInstance.placedTextIndex);
                const float fontSize = evaluateSizeForFeature(partiallyEvaluatedTextSize, placedSymbol);

                auto placed = collisionIndex.placeFeature(symbolInstance.textCollisionFeature,
                        posMatrix, textLabelPlaneMatrix, textPixelRatio,
                        placedSymbol, scale, fontSize,
                        bucket.layout.get<style::TextAllowOverlap>(),
                        bucket.layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map,
                        showCollisionBoxes);
                placeText = placed.first;
                offscreen &= placed.second;
            }

            if (symbolInstance.placedIconIndex) {
                PlacedSymbol& placedSymbol = bucket.icon.placedSymbols.at(*symbolInstance.placedIconIndex);
                const float fontSize = evaluateSizeForFeature(partiallyEvaluatedIconSize, placedSymbol);

                auto placed = collisionIndex.placeFeature(symbolInstance.iconCollisionFeature,
                        posMatrix, iconLabelPlaneMatrix, textPixelRatio,
                        placedSymbol, scale, fontSize,
                        bucket.layout.get<style::IconAllowOverlap>(),
                        bucket.layout.get<style::IconPitchAlignment>() == style::AlignmentType::Map,
                        showCollisionBoxes);
                placeIcon = placed.first;
                offscreen &= placed.second;
            }

            // combine placements for icon and text
            if (!iconWithoutText && !textWithoutIcon) {
                placeText = placeIcon = placeText && placeIcon;
            } else if (!textWithoutIcon) {
                placeText = placeText && placeIcon;
            } else if (!iconWithoutText) {
                placeIcon = placeText && placeIcon;
            }

            if (placeText) {
                collisionIndex.insertFeature(symbolInstance.textCollisionFeature, bucket.layout.get<style::TextIgnorePlacement>());
            }

            if (placeIcon) {
                collisionIndex.insertFeature(symbolInstance.iconCollisionFeature, bucket.layout.get<style::IconIgnorePlacement>());
            }

            assert(symbolInstance.crossTileID != 0);

            if (placements.find(symbolInstance.crossTileID) != placements.end()) {
                // If there's a previous placement with this ID, it comes from a tile that's fading out
                // Erase it so that the placement result from the non-fading tile supersedes it
                placements.erase(symbolInstance.crossTileID);
            }
            
            placements.emplace(symbolInstance.crossTileID, JointPlacement(placeText, placeIcon, offscreen || bucket.justReloaded));
            seenCrossTileIDs.insert(symbolInstance.crossTileID);
        }
    } 

    bucket.justReloaded = false;
}

bool Placement::commit(const Placement& prevPlacement, TimePoint now) {
    commitTime = now;

    bool placementChanged = false;

    float increment = mapMode == MapMode::Continuous ?
        std::chrono::duration<float>(commitTime - prevPlacement.commitTime) / Duration(std::chrono::milliseconds(300)) :
        1.0;

    // add the opacities from the current placement, and copy their current values from the previous placement
    for (auto& jointPlacement : placements) {
        auto prevOpacity = prevPlacement.opacities.find(jointPlacement.first);
        if (prevOpacity != prevPlacement.opacities.end()) {
            opacities.emplace(jointPlacement.first, JointOpacityState(prevOpacity->second, increment, jointPlacement.second.text, jointPlacement.second.icon));
            placementChanged = placementChanged ||
                jointPlacement.second.icon != prevOpacity->second.icon.placed ||
                jointPlacement.second.text != prevOpacity->second.text.placed;
        } else {
            opacities.emplace(jointPlacement.first, JointOpacityState(jointPlacement.second.text, jointPlacement.second.icon, jointPlacement.second.skipFade));
            placementChanged = placementChanged || jointPlacement.second.icon || jointPlacement.second.text;
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
    std::set<uint32_t> seenCrossTileIDs;
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

void Placement::updateBucketOpacities(SymbolBucket& bucket, std::set<uint32_t>& seenCrossTileIDs) {
    if (bucket.hasTextData()) bucket.text.opacityVertices.clear();
    if (bucket.hasIconData()) bucket.icon.opacityVertices.clear();
    if (bucket.hasCollisionBoxData()) bucket.collisionBox.dynamicVertices.clear();
    if (bucket.hasCollisionCircleData()) bucket.collisionCircle.dynamicVertices.clear();

    JointOpacityState duplicateOpacityState(false, false, true);

    JointOpacityState defaultOpacityState(
            bucket.layout.get<style::TextAllowOverlap>(),
            bucket.layout.get<style::IconAllowOverlap>(),
            true);

    for (SymbolInstance& symbolInstance : bucket.symbolInstances) {
        bool isDuplicate = seenCrossTileIDs.count(symbolInstance.crossTileID) > 0;

        auto it = opacities.find(symbolInstance.crossTileID);
        auto opacityState = defaultOpacityState;
        if (isDuplicate) {
            opacityState = duplicateOpacityState;
        } else if (it != opacities.end()) {
            opacityState = it->second;
        }

        if (it == opacities.end()) {
            opacities.emplace(symbolInstance.crossTileID, defaultOpacityState);
        }

        seenCrossTileIDs.insert(symbolInstance.crossTileID);

        if (symbolInstance.hasText) {
            auto opacityVertex = SymbolOpacityAttributes::vertex(opacityState.text.placed, opacityState.text.opacity);
            for (size_t i = 0; i < symbolInstance.horizontalGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            for (size_t i = 0; i < symbolInstance.verticalGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            if (symbolInstance.placedTextIndex) {
                bucket.text.placedSymbols[*symbolInstance.placedTextIndex].hidden = opacityState.isHidden();
            }
            if (symbolInstance.placedVerticalTextIndex) {
                bucket.text.placedSymbols[*symbolInstance.placedVerticalTextIndex].hidden = opacityState.isHidden();
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
                bucket.icon.placedSymbols[*symbolInstance.placedIconIndex].hidden = opacityState.isHidden();
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

float Placement::symbolFadeChange(TimePoint now) const {
    if (mapMode == MapMode::Continuous) {
        return std::chrono::duration<float>(now - commitTime) / Duration(std::chrono::milliseconds(300));
    } else {
        return 1.0;
    }
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

const CollisionIndex& Placement::getCollisionIndex() const {
    return collisionIndex;
}

} // namespace mbgl
