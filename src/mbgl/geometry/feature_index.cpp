#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/render_symbol_layer.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/map/query.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/filter_evaluator.hpp>
#include <mbgl/tile/geometry_tile.hpp>

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

static bool vectorContains(const std::vector<std::string>& vector, const std::string& s) {
    return std::find(vector.begin(), vector.end(), s) != vector.end();
}

static bool vectorsIntersect(const std::vector<std::string>& vectorA, const std::vector<std::string>& vectorB) {
    for (const auto& a : vectorA) {
        if (vectorContains(vectorB, a)) {
            return true;
        }
    }
    return false;
}

static bool topDown(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex > b.sortIndex;
}

static bool topDownSymbols(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex < b.sortIndex;
}

static int16_t getAdditionalQueryRadius(const RenderedQueryOptions& queryOptions,
                                        const style::Style& style,
                                        const GeometryTile& tile,
                                        const float pixelsToTileUnits) {

    // Determine the additional radius needed factoring in property functions
    float additionalRadius = 0;
    auto getQueryRadius = [&](const RenderLayer& layer) {
        auto bucket = tile.getBucket(layer);
        if (bucket) {
            additionalRadius = std::max(additionalRadius, bucket->getQueryRadius(layer) * pixelsToTileUnits);
        }
    };

    if (queryOptions.layerIDs) {
        for (const auto& layerID : *queryOptions.layerIDs) {
            RenderLayer* layer = style.getRenderLayer(layerID);
            if (layer) {
                getQueryRadius(*layer);
            }
        }
    } else {
        for (const RenderLayer* layer : style.getRenderLayers()) {
            getQueryRadius(*layer);
        }
    }

    return std::min<int16_t>(util::EXTENT, additionalRadius);
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
        const style::Style& style,
        const CollisionTile* collisionTile,
        const GeometryTile& tile) const {

    // Determine query radius
    const float pixelsToTileUnits = util::EXTENT / tileSize / scale;
    const int16_t additionalRadius = getAdditionalQueryRadius(queryOptions, style, tile, pixelsToTileUnits);

    // Query the grid index
    mapbox::geometry::box<int16_t> box = mapbox::geometry::envelope(queryGeometry);
    std::vector<IndexedSubfeature> features = grid.query({ box.min - additionalRadius, box.max + additionalRadius });


    std::sort(features.begin(), features.end(), topDown);
    size_t previousSortIndex = std::numeric_limits<size_t>::max();
    for (const auto& indexedFeature : features) {

        // If this feature is the same as the previous feature, skip it.
        if (indexedFeature.sortIndex == previousSortIndex) continue;
        previousSortIndex = indexedFeature.sortIndex;

        addFeature(result, indexedFeature, queryGeometry, queryOptions, geometryTileData, tileID, style, bearing, pixelsToTileUnits);
    }

    // Query symbol features, if they've been placed.
    if (!collisionTile) {
        return;
    }

    std::vector<IndexedSubfeature> symbolFeatures = collisionTile->queryRenderedSymbols(queryGeometry, scale);
    std::sort(symbolFeatures.begin(), symbolFeatures.end(), topDownSymbols);
    for (const auto& symbolFeature : symbolFeatures) {
        addFeature(result, symbolFeature, queryGeometry, queryOptions, geometryTileData, tileID, style, bearing, pixelsToTileUnits);
    }
}

void FeatureIndex::addFeature(
    std::unordered_map<std::string, std::vector<Feature>>& result,
    const IndexedSubfeature& indexedFeature,
    const GeometryCoordinates& queryGeometry,
    const RenderedQueryOptions& options,
    const GeometryTileData& geometryTileData,
    const CanonicalTileID& tileID,
    const style::Style& style,
    const float bearing,
    const float pixelsToTileUnits) const {

    auto& layerIDs = bucketLayerIDs.at(indexedFeature.bucketName);
    if (options.layerIDs && !vectorsIntersect(layerIDs, *options.layerIDs)) {
        return;
    }

    auto sourceLayer = geometryTileData.getLayer(indexedFeature.sourceLayerName);
    assert(sourceLayer);

    auto geometryTileFeature = sourceLayer->getFeature(indexedFeature.index);
    assert(geometryTileFeature);

    for (const auto& layerID : layerIDs) {
        if (options.layerIDs && !vectorContains(*options.layerIDs, layerID)) {
            continue;
        }

        auto renderLayer = style.getRenderLayer(layerID);
        if (!renderLayer ||
            (!renderLayer->is<RenderSymbolLayer>() &&
             !renderLayer->queryIntersectsFeature(queryGeometry, *geometryTileFeature, tileID.z, bearing, pixelsToTileUnits))) {
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
