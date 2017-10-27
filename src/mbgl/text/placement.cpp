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

uint32_t Placement::maxCrossTileID = 0;

Placement::Placement(const TransformState& state_) : collisionIndex(state_), state(state_) {}

void Placement::placeLayer(RenderSymbolLayer& symbolLayer, const mat4& projMatrix, bool showCollisionBoxes) {
    for (RenderTile& renderTile : symbolLayer.renderTiles) {

        if (!renderTile.tile.isRenderable()) {
            continue;
        }

        auto bucket = renderTile.tile.getBucket(*symbolLayer.baseImpl);
        assert(dynamic_cast<SymbolBucket*>(bucket));
        SymbolBucket& symbolBucket = *reinterpret_cast<SymbolBucket*>(bucket);

        auto& layout = symbolBucket.layout;

        const float pixelsToTileUnits = renderTile.id.pixelsToTileUnits(1, state.getZoom());

        const float scale = std::pow(2, state.getZoom() - renderTile.id.canonical.z);

        const float pixelRatio = util::EXTENT / (util::tileSize * renderTile.tile.id.overscaleFactor());

        mat4 posMatrix;
        state.matrixFor(posMatrix, renderTile.id);
        matrix::multiply(posMatrix, projMatrix, posMatrix);

        mat4 textLabelPlaneMatrix = getLabelPlaneMatrix(renderTile.matrix,
                layout.get<TextPitchAlignment>() == style::AlignmentType::Map,
                layout.get<TextRotationAlignment>() == style::AlignmentType::Map,
                state,
                pixelsToTileUnits);

        mat4 iconLabelPlaneMatrix = getLabelPlaneMatrix(renderTile.matrix,
                layout.get<IconPitchAlignment>() == style::AlignmentType::Map,
                layout.get<IconRotationAlignment>() == style::AlignmentType::Map,
                state,
                pixelsToTileUnits);

        placeLayerBucket(symbolBucket, posMatrix, textLabelPlaneMatrix, iconLabelPlaneMatrix, scale, pixelRatio, showCollisionBoxes);
    }
}

void Placement::placeLayerBucket(
        SymbolBucket& bucket,
        const mat4& posMatrix,
        const mat4& textLabelPlaneMatrix,
        const mat4& iconLabelPlaneMatrix,
        const float scale,
        const float pixelRatio,
        const bool showCollisionBoxes) {

    // TODO collision debug array clearing

    auto partiallyEvaluatedTextSize = bucket.textSizeBinder->evaluateForZoom(state.getZoom());
    auto partiallyEvaluatedIconSize = bucket.iconSizeBinder->evaluateForZoom(state.getZoom());

    const bool iconWithoutText = !bucket.hasTextData() || bucket.layout.get<TextOptional>();
    const bool textWithoutIcon = !bucket.hasIconData() || bucket.layout.get<IconOptional>();

    for (auto& symbolInstance : bucket.symbolInstances) {

        // TODO symbollayout handles a couple special cases related to this. copy those over if needed.
        auto anchor = symbolInstance.anchor;
        const bool withinPlus0 = anchor.point.x >= 0 && anchor.point.x < util::EXTENT && anchor.point.y >= 0 && anchor.point.y < util::EXTENT;
        if (!withinPlus0) continue;

        bool placeText = false;
        bool placeIcon = false;

        if (true || !symbolInstance.isDuplicate) {

            if (symbolInstance.placedTextIndices.size()) {
                assert(symbolInstance.placedTextIndices.size() != 0);

                PlacedSymbol& placedSymbol = bucket.text.placedSymbols.at(symbolInstance.placedTextIndices.at(0));
                const float fontSize = evaluateSizeForFeature(partiallyEvaluatedTextSize, placedSymbol);

                placeText = collisionIndex.placeFeature(symbolInstance.textCollisionFeature,
                        posMatrix, textLabelPlaneMatrix, pixelRatio,
                        placedSymbol, scale, fontSize,
                        bucket.layout.get<TextAllowOverlap>(),
                        bucket.layout.get<TextPitchAlignment>() == style::AlignmentType::Map,
                        showCollisionBoxes);
            }

            if (symbolInstance.placedIconIndices.size()) {

                PlacedSymbol& placedSymbol = bucket.icon.placedSymbols.at(symbolInstance.placedIconIndices.at(0));
                const float fontSize = evaluateSizeForFeature(partiallyEvaluatedIconSize, placedSymbol);

                placeIcon = collisionIndex.placeFeature(symbolInstance.iconCollisionFeature,
                        posMatrix, iconLabelPlaneMatrix, pixelRatio,
                        placedSymbol, scale, fontSize,
                        bucket.layout.get<IconAllowOverlap>(),
                        bucket.layout.get<IconPitchAlignment>() == style::AlignmentType::Map,
                        showCollisionBoxes);
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
                collisionIndex.insertFeature(symbolInstance.textCollisionFeature, bucket.layout.get<TextIgnorePlacement>());
            }

            if (placeIcon) {
                collisionIndex.insertFeature(symbolInstance.iconCollisionFeature, bucket.layout.get<IconIgnorePlacement>());
            }

            if (symbolInstance.crossTileID == 0) {
                // TODO properly assign these
                symbolInstance.crossTileID = ++maxCrossTileID;
            }

            placements.emplace(symbolInstance.crossTileID, PlacementPair(placeText, placeIcon));
        }
    } 
}

bool Placement::commit(const Placement& prevPlacement, TimePoint now) {
    commitTime = now;

    bool placementChanged = false;

    float increment = std::chrono::duration<float>(commitTime - prevPlacement.commitTime) / Duration(std::chrono::milliseconds(300));

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
    for (RenderTile& renderTile : symbolLayer.renderTiles) {
        if (!renderTile.tile.isRenderable()) {
            continue;
        }

        auto bucket = renderTile.tile.getBucket(*symbolLayer.baseImpl);
        assert(dynamic_cast<SymbolBucket*>(bucket));
        SymbolBucket& symbolBucket = *reinterpret_cast<SymbolBucket*>(bucket);
        updateBucketOpacities(symbolBucket);
    }
}

void Placement::updateBucketOpacities(SymbolBucket& bucket) {
    // TODO check if this clear is necessary, whether the vector has been moved out
    if (bucket.hasTextData()) bucket.text.opacityVertices.clear();
    if (bucket.hasIconData()) bucket.icon.opacityVertices.clear();
    if (bucket.hasCollisionBoxData()) bucket.collisionBox.opacityVertices.clear();
    if (bucket.hasCollisionCircleData()) bucket.collisionCircle.opacityVertices.clear();

    for (SymbolInstance& symbolInstance : bucket.symbolInstances) {
        auto opacityState = getOpacity(symbolInstance.crossTileID);

        // TODO check if hasText is the right thing here, or if there are cases where hasText is true but it's not added to the buffers
        if (symbolInstance.hasText) {
            auto opacityVertex = SymbolOpacityAttributes::vertex(opacityState.text.placed, opacityState.text.opacity);
            for (size_t i = 0; i < symbolInstance.horizontalGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            for (size_t i = 0; i < symbolInstance.verticalGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            // TODO On JS we avoid setting this if it hasn't chnaged, but it may be cheap enough here we don't care
            for (auto index : symbolInstance.placedTextIndices) {
                bucket.text.placedSymbols[index].hidden = opacityState.isHidden();
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
            // TODO On JS we avoid setting this if it hasn't chnaged, but it may be cheap enough here we don't care
            for (auto index : symbolInstance.placedIconIndices) {
                bucket.icon.placedSymbols[index].hidden = opacityState.isHidden();
            }
        }
        
        auto updateCollisionBox = [&](const auto& feature, const bool placed) {
            for (const CollisionBox& box : feature.boxes) {
                if (feature.alongLine) {
                   auto opacityVertex = CollisionBoxOpacityAttributes::vertex(placed, !box.used);
                    bucket.collisionCircle.opacityVertices.emplace_back(opacityVertex);
                    bucket.collisionCircle.opacityVertices.emplace_back(opacityVertex);
                    bucket.collisionCircle.opacityVertices.emplace_back(opacityVertex);
                    bucket.collisionCircle.opacityVertices.emplace_back(opacityVertex);
                } else {
                    auto opacityVertex = CollisionBoxOpacityAttributes::vertex(placed, false);
                    bucket.collisionBox.opacityVertices.emplace_back(opacityVertex);
                    bucket.collisionBox.opacityVertices.emplace_back(opacityVertex);
                    bucket.collisionBox.opacityVertices.emplace_back(opacityVertex);
                    bucket.collisionBox.opacityVertices.emplace_back(opacityVertex);
                }
            }
        };
        updateCollisionBox(symbolInstance.textCollisionFeature, opacityState.text.placed);
        updateCollisionBox(symbolInstance.iconCollisionFeature, opacityState.icon.placed);
    }

    bucket.updateOpacity();
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
    return std::chrono::duration<float>(now - commitTime) / Duration(std::chrono::milliseconds(300));
}

bool Placement::hasTransitions(TimePoint now) const {
    return symbolFadeChange(now) < 1.0;
}

} // namespace mbgl
