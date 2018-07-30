#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/text/collision_index.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <mapbox/geometry/envelope.hpp>

#include <cassert>
#include <string>

namespace mbgl {

FeatureIndex::FeatureIndex(std::unique_ptr<const GeometryTileData> tileData_)
    : grid(util::EXTENT, util::EXTENT, util::EXTENT / 16) // 16x16 grid -> 32px cell
    , tileData(std::move(tileData_)) {
}

void FeatureIndex::insert(const GeometryCollection& geometries,
                          std::size_t index,
                          const std::string& sourceLayerName,
                          const std::string& bucketLeaderID) {
    for (const auto& ring : geometries) {
        auto envelope = mapbox::geometry::envelope(ring);
        if (envelope.min.x < util::EXTENT &&
            envelope.min.y < util::EXTENT &&
            envelope.max.x >= 0 &&
            envelope.max.y >= 0) {
            grid.insert(IndexedSubfeature(index, sourceLayerName, bucketLeaderID, sortIndex++),
                        {convertPoint<float>(envelope.min), convertPoint<float>(envelope.max)});
        }
    }
}

void FeatureIndex::query(
        std::unordered_map<std::string, std::vector<Feature>>& result,
        const GeometryCoordinates& queryGeometry,
        const TransformState& transformState,
        const mat4& posMatrix,
        const double tileSize,
        const double scale,
        const RenderedQueryOptions& queryOptions,
        const UnwrappedTileID& tileID,
        const std::vector<const RenderLayer*>& layers,
        const float additionalQueryPadding) const {
    
    if (!tileData) {
        return;
    }

    // Determine query radius
    const float pixelsToTileUnits = util::EXTENT / tileSize / scale;
    const int16_t additionalPadding = std::min<int16_t>(util::EXTENT, additionalQueryPadding * pixelsToTileUnits);

    // Query the grid index
    mapbox::geometry::box<int16_t> box = mapbox::geometry::envelope(queryGeometry);
    std::vector<IndexedSubfeature> features = grid.query({ convertPoint<float>(box.min - additionalPadding),
                                                           convertPoint<float>(box.max + additionalPadding) });


    std::sort(features.begin(), features.end(), [](const IndexedSubfeature& a, const IndexedSubfeature& b) {
        return a.sortIndex > b.sortIndex;
    });
    size_t previousSortIndex = std::numeric_limits<size_t>::max();
    for (const auto& indexedFeature : features) {

        // If this feature is the same as the previous feature, skip it.
        if (indexedFeature.sortIndex == previousSortIndex) continue;
        previousSortIndex = indexedFeature.sortIndex;

        addFeature(result, indexedFeature, queryOptions, tileID.canonical, layers, queryGeometry, transformState, pixelsToTileUnits, posMatrix);
    }
}
    
std::unordered_map<std::string, std::vector<Feature>>
FeatureIndex::lookupSymbolFeatures(const std::vector<IndexedSubfeature>& symbolFeatures,
                                   const RenderedQueryOptions& queryOptions,
                                   const std::vector<const RenderLayer*>& layers,
                                   const OverscaledTileID& tileID,
                                   const std::shared_ptr<std::vector<size_t>>& featureSortOrder) const {
    std::unordered_map<std::string, std::vector<Feature>> result;
    if (!tileData) {
        return result;
    }
    std::vector<IndexedSubfeature> sortedFeatures(symbolFeatures.begin(), symbolFeatures.end());

    std::sort(sortedFeatures.begin(), sortedFeatures.end(), [featureSortOrder](const IndexedSubfeature& a, const IndexedSubfeature& b) {
        // Same idea as the non-symbol sort order, but symbol features may have changed their sort order
        // since their corresponding IndexedSubfeature was added to the CollisionIndex
        // The 'featureSortOrder' is relatively inefficient for querying but cheap to build on every bucket sort
        if (featureSortOrder) {
            // queryRenderedSymbols documentation says we'll return features in
            // "top-to-bottom" rendering order (aka last-to-first).
            // Actually there can be multiple symbol instances per feature, so
            // we sort each feature based on the first matching symbol instance.
            auto sortedA = std::find(featureSortOrder->begin(), featureSortOrder->end(), a.index);
            auto sortedB = std::find(featureSortOrder->begin(), featureSortOrder->end(), b.index);
            assert(sortedA != featureSortOrder->end());
            assert(sortedB != featureSortOrder->end());
            return sortedA > sortedB;
        } else {
            // Bucket hasn't been re-sorted based on angle, so use same "reverse of appearance in source data"
            // logic as non-symboles
            return a.sortIndex > b.sortIndex;
        }
    });

    for (const auto& symbolFeature : sortedFeatures) {
        mat4 unusedMatrix;
        addFeature(result, symbolFeature, queryOptions, tileID.canonical, layers, GeometryCoordinates(), {}, 0, unusedMatrix);
    }
    return result;
}

void FeatureIndex::addFeature(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const IndexedSubfeature& indexedFeature,
    const RenderedQueryOptions& options,
    const CanonicalTileID& tileID,
    const std::vector<const RenderLayer*>& layers,
    const GeometryCoordinates& queryGeometry,
    const TransformState& transformState,
    const float pixelsToTileUnits,
    const mat4& posMatrix) const {

    auto getRenderLayer = [&] (const std::string& layerID) -> const RenderLayer* {
        for (const auto& layer : layers) {
            if (layer->getID() == layerID) {
                return layer;
            }
        }
        return nullptr;
    };

    // Lazily calculated.
    std::unique_ptr<GeometryTileLayer> sourceLayer;
    std::unique_ptr<GeometryTileFeature> geometryTileFeature;

    for (const std::string& layerID : bucketLayerIDs.at(indexedFeature.bucketLeaderID)) {
        const RenderLayer* renderLayer = getRenderLayer(layerID);
        if (!renderLayer) {
            continue;
        }

        if (!geometryTileFeature) {
            sourceLayer = tileData->getLayer(indexedFeature.sourceLayerName);
            assert(sourceLayer);

            geometryTileFeature = sourceLayer->getFeature(indexedFeature.index);
            assert(geometryTileFeature);
        }

        if (!renderLayer->is<RenderSymbolLayer>() &&
             !renderLayer->queryIntersectsFeature(queryGeometry, *geometryTileFeature, tileID.z, transformState, pixelsToTileUnits, posMatrix)) {
            continue;
        }

        if (options.filter && !(*options.filter)(style::expression::EvaluationContext { static_cast<float>(tileID.z), geometryTileFeature.get() })) {
            continue;
        }

        result[layerID].push_back(convertFeature(*geometryTileFeature, tileID));
    }
}

optional<GeometryCoordinates> FeatureIndex::translateQueryGeometry(
        const GeometryCoordinates& queryGeometry,
        const std::array<float, 2>& translate,
        const style::TranslateAnchorType anchorType,
        const float bearing,
        const float pixelsToTileUnits) {
    if (translate[0] == 0 && translate[1] == 0) {
        return {};
    }

    GeometryCoordinate translateVec(translate[0] * pixelsToTileUnits, translate[1] * pixelsToTileUnits);
    if (anchorType == style::TranslateAnchorType::Viewport) {
        translateVec = util::rotate(translateVec, -bearing);
    }

    GeometryCoordinates translated;
    for (const auto& p : queryGeometry) {
        translated.push_back(p - translateVec);
    }
    return translated;
}

void FeatureIndex::setBucketLayerIDs(const std::string& bucketLeaderID, const std::vector<std::string>& layerIDs) {
    bucketLayerIDs[bucketLeaderID] = layerIDs;
}

} // namespace mbgl
