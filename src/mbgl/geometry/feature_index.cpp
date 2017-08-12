#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>

#include <mapbox/geometry/envelope.hpp>

#include <cassert>
#include <string>

namespace mbgl {

FeatureIndex::FeatureIndex()
    : grid(util::EXTENT, 16, 0) {
}

void FeatureIndex::insert(const GeometryCollection& geometries,
                          std::size_t index,
                          const std::string& sourceLayerName,
                          const std::string& bucketName) {
    for (const auto& ring : geometries) {
        grid.insert(IndexedSubfeature { index, sourceLayerName, bucketName, sortIndex++ },
                    mapbox::geometry::envelope(ring));
    }
}

static bool topDown(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex > b.sortIndex;
}

static bool topDownSymbols(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex < b.sortIndex;
}

void FeatureIndex::query(
        std::unordered_map<std::string, std::vector<Feature>>& result,
        const GeometryCoordinates& queryGeometry,
        const float bearing,
        const double tileSize,
        const double scale,
        const RenderedQueryOptions& queryOptions,
        const GeometryTileData& geometryTileData,
        const CanonicalTileID& tileID,
        const std::vector<const RenderLayer*>& layers,
        const CollisionTile* collisionTile,
        const float additionalQueryRadius) const {

    // Determine query radius
    const float pixelsToTileUnits = util::EXTENT / tileSize / scale;
    const int16_t additionalRadius = std::min<int16_t>(util::EXTENT, additionalQueryRadius * pixelsToTileUnits);

    // Query the grid index
    mapbox::geometry::box<int16_t> box = mapbox::geometry::envelope(queryGeometry);
    std::vector<IndexedSubfeature> features = grid.query({ box.min - additionalRadius, box.max + additionalRadius });


    std::sort(features.begin(), features.end(), topDown);
    size_t previousSortIndex = std::numeric_limits<size_t>::max();
    for (const auto& indexedFeature : features) {

        // If this feature is the same as the previous feature, skip it.
        if (indexedFeature.sortIndex == previousSortIndex) continue;
        previousSortIndex = indexedFeature.sortIndex;

        addFeature(result, indexedFeature, queryGeometry, queryOptions, geometryTileData, tileID, layers, bearing, pixelsToTileUnits);
    }

    // Query symbol features, if they've been placed.
    if (!collisionTile) {
        return;
    }

    std::vector<IndexedSubfeature> symbolFeatures = collisionTile->queryRenderedSymbols(queryGeometry, scale);
    std::sort(symbolFeatures.begin(), symbolFeatures.end(), topDownSymbols);
    for (const auto& symbolFeature : symbolFeatures) {
        addFeature(result, symbolFeature, queryGeometry, queryOptions, geometryTileData, tileID, layers, bearing, pixelsToTileUnits);
    }
}

void FeatureIndex::addFeature(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const IndexedSubfeature& indexedFeature,
    const GeometryCoordinates& queryGeometry,
    const RenderedQueryOptions& options,
    const GeometryTileData& geometryTileData,
    const CanonicalTileID& tileID,
    const std::vector<const RenderLayer*>& layers,
    const float bearing,
    const float pixelsToTileUnits) const {

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

    for (const std::string& layerID : bucketLayerIDs.at(indexedFeature.bucketName)) {
        const RenderLayer* renderLayer = getRenderLayer(layerID);
        if (!renderLayer) {
            continue;
        }

        if (!geometryTileFeature) {
            sourceLayer = geometryTileData.getLayer(indexedFeature.sourceLayerName);
            assert(sourceLayer);

            geometryTileFeature = sourceLayer->getFeature(indexedFeature.index);
            assert(geometryTileFeature);
        }

        if (!renderLayer->is<RenderSymbolLayer>() &&
             !renderLayer->queryIntersectsFeature(queryGeometry, *geometryTileFeature, tileID.z, bearing, pixelsToTileUnits)) {
            continue;
        }

        if (options.filter && !(*options.filter)(*geometryTileFeature)) {
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

void FeatureIndex::setBucketLayerIDs(const std::string& bucketName, const std::vector<std::string>& layerIDs) {
    bucketLayerIDs[bucketName] = layerIDs;
}

} // namespace mbgl
