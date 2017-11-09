#include <mbgl/text/placement_worker.hpp>
#include <mbgl/layout/symbol_projection.hpp>

namespace mbgl {

PlacementBucket::PlacementBucket(Immutable<SymbolBucket::Core>& core, Immutable<std::vector<uint32_t>>& ids, const UnwrappedTileID& unwrapped, const OverscaledTileID& overscaled) :
    symbolBucketCore(core), symbolCrossTileIDs(ids), unwrappedTileID(unwrapped), overscaledTileID(overscaled) {}

PlacementWorker::PlacementWorker(const TransformState& state_, const mat4& projMatrix_, const bool showCollisionBoxes_) :
    state(state_),
    projMatrix(projMatrix_),
    showCollisionBoxes(showCollisionBoxes_)
    {}

Immutable<PlacementResult> PlacementWorker::place(Immutable<std::vector<std::vector<PlacementBucket>>> placementBucketLayers) {
    Mutable<PlacementResult> result = makeMutable<PlacementResult>(state);

    for (auto& placementBuckets : *placementBucketLayers) {
        std::unordered_set<uint32_t> seenCrossTileIDs;
        for (auto& placementBucket: placementBuckets) {
            placeLayerBucket(result->placements, result->collisionIndex, *placementBucket.symbolBucketCore, *placementBucket.symbolCrossTileIDs, placementBucket.unwrappedTileID, placementBucket.overscaledTileID, seenCrossTileIDs);
        }
    }

    return std::move(result);
}

void PlacementWorker::placeLayerBucket(
        std::unordered_map<uint32_t,PlacementPair>& placements,
        CollisionIndex& collisionIndex,
        const SymbolBucket::Core& bucketCore,
        const std::vector<uint32_t>& symbolCrossTileIDs,
        const UnwrappedTileID& unwrappedTileID,
        const OverscaledTileID& overscaledTileID,
        std::unordered_set<uint32_t>& seenCrossTileIDs) {

    const float scale = std::pow(2, state.getZoom() - overscaledTileID.overscaledZ);
    const float textPixelRatio = util::EXTENT / (util::tileSize * overscaledTileID.overscaleFactor());

    const float pixelsToTileUnits = unwrappedTileID.pixelsToTileUnits(1, state.getZoom());

    mat4 posMatrix;
    state.matrixFor(posMatrix, unwrappedTileID);
    matrix::multiply(posMatrix, projMatrix, posMatrix);

    mat4 textLabelPlaneMatrix = getLabelPlaneMatrix(posMatrix,
            bucketCore.layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map,
            bucketCore.layout.get<style::TextRotationAlignment>() == style::AlignmentType::Map,
            state,
            pixelsToTileUnits);

    mat4 iconLabelPlaneMatrix = getLabelPlaneMatrix(posMatrix,
            bucketCore.layout.get<style::IconPitchAlignment>() == style::AlignmentType::Map,
            bucketCore.layout.get<style::IconRotationAlignment>() == style::AlignmentType::Map,
            state,
            pixelsToTileUnits);

    auto partiallyEvaluatedTextSize = bucketCore.textSizeBinder->evaluateForZoom(state.getZoom());
    auto partiallyEvaluatedIconSize = bucketCore.iconSizeBinder->evaluateForZoom(state.getZoom());

    const bool iconWithoutText = !bucketCore.hasTextData || bucketCore.layout.get<style::TextOptional>();
    const bool textWithoutIcon = !bucketCore.hasIconData || bucketCore.layout.get<style::IconOptional>();

    assert(bucketCore.symbolInstances.size() == symbolCrossTileIDs.size());

    auto idIt = symbolCrossTileIDs.begin();
    for (auto it = bucketCore.symbolInstances.begin(); it != bucketCore.symbolInstances.end(); it++, idIt++) {
        auto& symbolInstance = *it;
        const uint32_t crossTileID = *idIt;

        if (seenCrossTileIDs.count(crossTileID) == 0) {
            bool placeText = false;
            bool placeIcon = false;

            // TODO eliminate this copy
            CollisionFeature textCollisionFeature = symbolInstance.textCollisionFeature;
            if (symbolInstance.placedTextIndex) {
                const PlacedSymbol& placedSymbol = bucketCore.placedTextSymbols.at(*symbolInstance.placedTextIndex);
                const float fontSize = evaluateSizeForFeature(partiallyEvaluatedTextSize, placedSymbol);

                placeText = collisionIndex.placeFeature(textCollisionFeature,
                        posMatrix, textLabelPlaneMatrix, textPixelRatio,
                        placedSymbol, scale, fontSize,
                        bucketCore.layout.get<style::TextAllowOverlap>(),
                        bucketCore.layout.get<style::TextPitchAlignment>() == style::AlignmentType::Map,
                        showCollisionBoxes);
            }

            // TODO eliminate this copy
            CollisionFeature iconCollisionFeature = symbolInstance.iconCollisionFeature;
            if (symbolInstance.placedIconIndex) {
                const PlacedSymbol& placedSymbol = bucketCore.placedIconSymbols.at(*symbolInstance.placedIconIndex);
                const float fontSize = evaluateSizeForFeature(partiallyEvaluatedIconSize, placedSymbol);

                placeIcon = collisionIndex.placeFeature(iconCollisionFeature,
                        posMatrix, iconLabelPlaneMatrix, textPixelRatio,
                        placedSymbol, scale, fontSize,
                        bucketCore.layout.get<style::IconAllowOverlap>(),
                        bucketCore.layout.get<style::IconPitchAlignment>() == style::AlignmentType::Map,
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
                collisionIndex.insertFeature(textCollisionFeature, bucketCore.layout.get<style::TextIgnorePlacement>());
            }

            if (placeIcon) {
                collisionIndex.insertFeature(iconCollisionFeature, bucketCore.layout.get<style::IconIgnorePlacement>());
            }

            assert(crossTileID != 0);

            placements.emplace(crossTileID, PlacementPair(placeText, placeIcon));
            seenCrossTileIDs.insert(crossTileID);
        }
    }
}

} // namespace mbgl
