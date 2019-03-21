#include <mbgl/text/placement.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_layer_symbol_interface.hpp>
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
    
const CollisionGroups::CollisionGroup& CollisionGroups::get(const std::string& sourceID) {
    // The predicate/groupID mechanism allows for arbitrary grouping,
    // but the current interface defines one source == one group when
    // crossSourceCollisions == true.
    if (!crossSourceCollisions) {
        if (collisionGroups.find(sourceID) == collisionGroups.end()) {
            uint16_t nextGroupID = ++maxGroupID;
            collisionGroups.emplace(sourceID, CollisionGroup(
                nextGroupID,
                optional<Predicate>([nextGroupID](const IndexedSubfeature& feature) -> bool {
                    return feature.collisionGroupId == nextGroupID;
                })
            ));
        }
        return collisionGroups[sourceID];
    } else {
        static CollisionGroup nullGroup{0, nullopt};
        return nullGroup;
    }
}

Placement::Placement(const TransformState& state_, MapMode mapMode_, style::TransitionOptions transitionOptions_, const bool crossSourceCollisions)
    : collisionIndex(state_)
    , state(state_)
    , mapMode(mapMode_)
    , transitionOptions(transitionOptions_)
    , collisionGroups(crossSourceCollisions)
{}

void Placement::placeLayer(const RenderLayerSymbolInterface& symbolInterface, const mat4& projMatrix, bool showCollisionBoxes) {

    std::unordered_set<uint32_t> seenCrossTileIDs;

    for (const RenderTile& renderTile : symbolInterface.getRenderTiles()) {
        if (!renderTile.tile.isRenderable()) {
            continue;
        }
        assert(renderTile.tile.kind == Tile::Kind::Geometry);
        GeometryTile& geometryTile = static_cast<GeometryTile&>(renderTile.tile);

        auto bucket = symbolInterface.getSymbolBucket(renderTile);
        if (!bucket) {
            continue;
        }
        SymbolBucket& symbolBucket = *bucket;

        if (symbolBucket.bucketLeaderID != symbolInterface.layerID()) {
            // Only place this layer if it's the "group leader" for the bucket
            continue;
        }

        auto& layout = symbolBucket.layout;

        const float pixelsToTileUnits = renderTile.id.pixelsToTileUnits(1, state.getZoom());

        const float scale = std::pow(2, state.getZoom() - geometryTile.id.overscaledZ);
        const float textPixelRatio = (util::tileSize * geometryTile.id.overscaleFactor()) / util::EXTENT;

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
        
        
        // As long as this placement lives, we have to hold onto this bucket's
        // matching FeatureIndex/data for querying purposes
        retainedQueryData.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(symbolBucket.bucketInstanceId),
                                  std::forward_as_tuple(symbolBucket.bucketInstanceId, geometryTile.getFeatureIndex(), geometryTile.id));
        
        const auto collisionGroup = collisionGroups.get(geometryTile.sourceID);
        
        placeLayerBucket(symbolBucket, posMatrix, textLabelPlaneMatrix, iconLabelPlaneMatrix, scale, textPixelRatio, showCollisionBoxes, seenCrossTileIDs, renderTile.tile.holdForFade(), collisionGroup);
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
        const bool holdingForFade,
        const CollisionGroups::CollisionGroup& collisionGroup) {

    auto partiallyEvaluatedTextSize = bucket.textSizeBinder->evaluateForZoom(state.getZoom());
    auto partiallyEvaluatedIconSize = bucket.iconSizeBinder->evaluateForZoom(state.getZoom());

    optional<CollisionTileBoundaries> avoidEdges;
    if (mapMode == MapMode::Tile &&
        (bucket.layout.get<style::SymbolAvoidEdges>() ||
         bucket.layout.get<style::SymbolPlacement>() == style::SymbolPlacementType::Line)) {
        avoidEdges = collisionIndex.projectTileBoundaries(posMatrix);
    }
    
    const bool textAllowOverlap = bucket.layout.get<style::TextAllowOverlap>();
    const bool iconAllowOverlap = bucket.layout.get<style::IconAllowOverlap>();
    // This logic is similar to the "defaultOpacityState" logic below in updateBucketOpacities
    // If we know a symbol is always supposed to show, force it to be marked visible even if
    // it wasn't placed into the collision index (because some or all of it was outside the range
    // of the collision grid).
    // There is a subtle edge case here we're accepting:
    //  Symbol A has text-allow-overlap: true, icon-allow-overlap: true, icon-optional: false
    //  A's icon is outside the grid, so doesn't get placed
    //  A's text would be inside grid, but doesn't get placed because of icon-optional: false
    //  We still show A because of the allow-overlap settings.
    //  Symbol B has allow-overlap: false, and gets placed where A's text would be
    //  On panning in, there is a short period when Symbol B and Symbol A will overlap
    //  This is the reverse of our normal policy of "fade in on pan", but should look like any other
    //  collision and hopefully not be too noticeable.
    // See https://github.com/mapbox/mapbox-gl-native/issues/12683
    const bool alwaysShowText = textAllowOverlap && (iconAllowOverlap || !bucket.hasIconData() || bucket.layout.get<style::IconOptional>());
    const bool alwaysShowIcon = iconAllowOverlap && (textAllowOverlap || !bucket.hasTextData() || bucket.layout.get<style::TextOptional>());
    
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

            if (symbolInstance.placedRightTextIndex) {
                PlacedSymbol& placedSymbol = bucket.text.placedSymbols.at(*symbolInstance.placedRightTextIndex);
                const float fontSize = evaluateSizeForFeature(partiallyEvaluatedTextSize, placedSymbol);

                auto placed = collisionIndex.placeFeature(symbolInstance.textCollisionFeature,
                        posMatrix, textLabelPlaneMatrix, textPixelRatio,
                        placedSymbol, scale, fontSize,
                        bucket.layout.get<style::TextAllowOverlap>(),
                        bucket.layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map,
                        showCollisionBoxes, avoidEdges, collisionGroup.second);
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
                        showCollisionBoxes, avoidEdges, collisionGroup.second);
                placeIcon = placed.first;
                offscreen &= placed.second;
            }

            const bool iconWithoutText = !symbolInstance.hasText || bucket.layout.get<style::TextOptional>();
            const bool textWithoutIcon = !symbolInstance.hasIcon || bucket.layout.get<style::IconOptional>();

            // combine placements for icon and text
            if (!iconWithoutText && !textWithoutIcon) {
                placeText = placeIcon = placeText && placeIcon;
            } else if (!textWithoutIcon) {
                placeText = placeText && placeIcon;
            } else if (!iconWithoutText) {
                placeIcon = placeText && placeIcon;
            }

            if (placeText) {
                collisionIndex.insertFeature(symbolInstance.textCollisionFeature, bucket.layout.get<style::TextIgnorePlacement>(), bucket.bucketInstanceId, collisionGroup.first);
            }

            if (placeIcon) {
                collisionIndex.insertFeature(symbolInstance.iconCollisionFeature, bucket.layout.get<style::IconIgnorePlacement>(), bucket.bucketInstanceId, collisionGroup.first);
            }

            assert(symbolInstance.crossTileID != 0);

            if (placements.find(symbolInstance.crossTileID) != placements.end()) {
                // If there's a previous placement with this ID, it comes from a tile that's fading out
                // Erase it so that the placement result from the non-fading tile supersedes it
                placements.erase(symbolInstance.crossTileID);
            }
            
            placements.emplace(symbolInstance.crossTileID, JointPlacement(placeText || alwaysShowText, placeIcon || alwaysShowIcon, offscreen || bucket.justReloaded));
            seenCrossTileIDs.insert(symbolInstance.crossTileID);
        }
    } 

    bucket.justReloaded = false;
}

void Placement::commit(const Placement& prevPlacement, TimePoint now) {
    commitTime = now;

    bool placementChanged = false;

    float increment = mapMode == MapMode::Continuous &&
                      transitionOptions.enablePlacementTransitions &&
                      transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION) > Milliseconds(0) ?
        std::chrono::duration<float>(commitTime - prevPlacement.commitTime) / transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION) :
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

    fadeStartTime = placementChanged ? commitTime : prevPlacement.fadeStartTime;
}

void Placement::updateLayerOpacities(const RenderLayerSymbolInterface& symbolInterface) {
    std::set<uint32_t> seenCrossTileIDs;
    for (const RenderTile& renderTile : symbolInterface.getRenderTiles()) {
        if (!renderTile.tile.isRenderable()) {
            continue;
        }

        auto bucket = symbolInterface.getSymbolBucket(renderTile);
        if (!bucket) {
            continue;
        }
        SymbolBucket& symbolBucket = *bucket;

        if (symbolBucket.bucketLeaderID != symbolInterface.layerID()) {
            // Only update opacities this layer if it's the "group leader" for the bucket
            continue;
        }
        updateBucketOpacities(symbolBucket, seenCrossTileIDs);
    }
}

void Placement::updateBucketOpacities(SymbolBucket& bucket, std::set<uint32_t>& seenCrossTileIDs) {
    if (bucket.hasTextData()) bucket.text.opacityVertices.clear();
    if (bucket.hasIconData()) bucket.icon.opacityVertices.clear();
    if (bucket.hasCollisionBoxData()) bucket.collisionBox.dynamicVertices.clear();
    if (bucket.hasCollisionCircleData()) bucket.collisionCircle.dynamicVertices.clear();

    JointOpacityState duplicateOpacityState(false, false, true);

    const bool textAllowOverlap = bucket.layout.get<style::TextAllowOverlap>();
    const bool iconAllowOverlap = bucket.layout.get<style::IconAllowOverlap>();
    
    // If allow-overlap is true, we can show symbols before placement runs on them
    // But we have to wait for placement if we potentially depend on a paired icon/text
    // with allow-overlap: false.
    // See https://github.com/mapbox/mapbox-gl-native/issues/12483
    JointOpacityState defaultOpacityState(
            textAllowOverlap && (iconAllowOverlap || !bucket.hasIconData() || bucket.layout.get<style::IconOptional>()),
            iconAllowOverlap && (textAllowOverlap || !bucket.hasTextData() || bucket.layout.get<style::TextOptional>()),
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
            auto opacityVertex = SymbolSDFTextProgram::opacityVertex(opacityState.text.placed, opacityState.text.opacity);
            for (size_t i = 0; i < symbolInstance.rightJustifiedGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            for (size_t i = 0; i < symbolInstance.verticalGlyphQuads.size() * 4; i++) {
                bucket.text.opacityVertices.emplace_back(opacityVertex);
            }
            if (symbolInstance.placedRightTextIndex) {
                PlacedSymbol& placed = bucket.text.placedSymbols[*symbolInstance.placedRightTextIndex];
                placed.hidden = opacityState.isHidden();
            }
            if (symbolInstance.placedVerticalTextIndex) {
                bucket.text.placedSymbols[*symbolInstance.placedVerticalTextIndex].hidden = opacityState.isHidden();
            }
        }
        if (symbolInstance.hasIcon) {
            auto opacityVertex = SymbolIconProgram::opacityVertex(opacityState.icon.placed, opacityState.icon.opacity);
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
            if (feature.alongLine) {
                return;
            }
            auto dynamicVertex = CollisionBoxProgram::dynamicVertex(placed, false);
            for (size_t i = 0; i < feature.boxes.size() * 4; i++) {
                bucket.collisionBox.dynamicVertices.emplace_back(dynamicVertex);
            }
        };
        
        auto updateCollisionCircles = [&](const auto& feature, const bool placed) {
            if (!feature.alongLine) {
                return;
            }
            for (const CollisionBox& box : feature.boxes) {
                auto dynamicVertex = CollisionBoxProgram::dynamicVertex(placed, !box.used);
                bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
                bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
                bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
                bucket.collisionCircle.dynamicVertices.emplace_back(dynamicVertex);
            }
        };
        
        if (bucket.hasCollisionBoxData()) {
            updateCollisionBox(symbolInstance.textCollisionFeature, opacityState.text.placed);
            updateCollisionBox(symbolInstance.iconCollisionFeature, opacityState.icon.placed);
        }
        if (bucket.hasCollisionCircleData()) {
            updateCollisionCircles(symbolInstance.textCollisionFeature, opacityState.text.placed);
            updateCollisionCircles(symbolInstance.iconCollisionFeature, opacityState.icon.placed);
        }
    }

    bucket.updateOpacity();
    bucket.sortFeatures(state.getBearing());
    auto retainedData = retainedQueryData.find(bucket.bucketInstanceId);
    if (retainedData != retainedQueryData.end()) {
        retainedData->second.featureSortOrder = bucket.featureSortOrder;
    }
}

float Placement::symbolFadeChange(TimePoint now) const {
    if (mapMode == MapMode::Continuous && transitionOptions.enablePlacementTransitions &&
        transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION) > Milliseconds(0)) {
        return std::chrono::duration<float>(now - commitTime) / transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION);
    } else {
        return 1.0;
    }
}

bool Placement::hasTransitions(TimePoint now) const {
    if (mapMode == MapMode::Continuous && transitionOptions.enablePlacementTransitions) {
        return stale || std::chrono::duration<float>(now - fadeStartTime) < transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION);
    } else {
        return false;
    }
}

bool Placement::stillRecent(TimePoint now) const {
    // Even if transitionOptions.duration is set to a value < 300ms, we still wait for this default transition duration
    // before attempting another placement operation.
    return mapMode == MapMode::Continuous &&
        transitionOptions.enablePlacementTransitions &&
        commitTime + std::max(util::DEFAULT_TRANSITION_DURATION, transitionOptions.duration.value_or(util::DEFAULT_TRANSITION_DURATION)) > now;
}

void Placement::setStale() {
    stale = true;
}

const CollisionIndex& Placement::getCollisionIndex() const {
    return collisionIndex;
}
    
const RetainedQueryData& Placement::getQueryData(uint32_t bucketInstanceId) const {
    auto it = retainedQueryData.find(bucketInstanceId);
    if (it == retainedQueryData.end()) {
        throw std::runtime_error("Placement::getQueryData with unrecognized bucketInstanceId");
    }
    return it->second;
}

} // namespace mbgl
