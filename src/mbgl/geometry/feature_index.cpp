#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/util/get_geometries.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/util/rapidjson.hpp>
#include <rapidjson/writer.h>
#include <mbgl/util/constants.hpp>

#include <cassert>
#include <string>

using namespace mbgl;

FeatureIndex::FeatureIndex() : grid(util::EXTENT, 16, 0) {}

void FeatureIndex::insert(const GeometryCollection& geometries, std::size_t index,
        const std::string& sourceLayerName, const std::string& bucketName) {

    for (auto& ring : geometries) {

        float minX = std::numeric_limits<float>::infinity();
        float minY = std::numeric_limits<float>::infinity();
        float maxX = -std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();
        for (auto& p : ring) {
            const float x = p.x;
            const float y = p.y;
            minX = util::min(minX, x);
            minY = util::min(minY, y);
            maxX = util::max(maxX, x);
            maxY = util::max(maxY, y);
        }

        grid.insert(
                IndexedSubfeature { index, sourceLayerName, bucketName, sortIndex++ },
                { int32_t(minX), int32_t(minY), int32_t(maxX), int32_t(maxY) });
    }
}

bool vectorContains(const std::vector<std::string>& vector, const std::string& s) {
    return std::find(vector.begin(), vector.end(), s) != vector.end();
}

bool vectorsIntersect(const std::vector<std::string>& vectorA, const std::vector<std::string>& vectorB) {
    for (auto& a : vectorA) {
        if (vectorContains(vectorB, a)) return true;;
    }
    return false;
}


bool topDown(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex > b.sortIndex;
}

bool topDownSymbols(const IndexedSubfeature& a, const IndexedSubfeature& b) {
    return a.sortIndex < b.sortIndex;
}

void FeatureIndex::query(
        std::unordered_map<std::string, std::vector<std::string>>& result,
        const GeometryCollection& queryGeometry,
        const float bearing,
        const double tileSize,
        const double scale,
        const optional<std::vector<std::string>>& filterLayerIDs,
        const GeometryTile& geometryTile,
        const Style& style) const {

    const float pixelsToTileUnits = util::EXTENT / tileSize / scale;

    float additionalRadius = style.getQueryRadius() * pixelsToTileUnits;

    float minX = std::numeric_limits<float>::infinity();
    float minY = std::numeric_limits<float>::infinity();
    float maxX = -std::numeric_limits<float>::infinity();
    float maxY = -std::numeric_limits<float>::infinity();

    for (auto& ring : queryGeometry) {
        for (auto& p : ring) {
            minX = util::min<float>(minX, p.x);
            minY = util::min<float>(minY, p.y);
            maxX = util::max<float>(maxX, p.x);
            maxY = util::max<float>(maxY, p.y);
        }
    }

    std::vector<IndexedSubfeature> features = grid.query({
        int(minX - additionalRadius),
        int(minY - additionalRadius),
        int(maxX + additionalRadius),
        int(maxY + additionalRadius)
    });

    std::sort(features.begin(), features.end(), topDown);
    size_t previousSortIndex = std::numeric_limits<size_t>::max();
    for (auto& indexedFeature : features) {

        // If this feature is the same as the previous feature, skip it.
        if (indexedFeature.sortIndex == previousSortIndex) continue;
        previousSortIndex = indexedFeature.sortIndex;

        addFeature(result, indexedFeature, queryGeometry, filterLayerIDs, geometryTile, style, bearing, pixelsToTileUnits);
    }

    // query symbol features
    assert(collisionTile);
    std::vector<IndexedSubfeature> symbolFeatures = collisionTile->queryRenderedSymbols(minX, minY, maxX, maxY, scale);
    std::sort(symbolFeatures.begin(), symbolFeatures.end(), topDownSymbols);
    for (auto& symbolFeature : symbolFeatures) {
        addFeature(result, symbolFeature, queryGeometry, filterLayerIDs, geometryTile, style, bearing, pixelsToTileUnits);
    }
}

void FeatureIndex::addFeature(
    std::unordered_map<std::string, std::vector<std::string>>& result,
    const IndexedSubfeature& indexedFeature,
    const GeometryCollection& queryGeometry,
    const optional<std::vector<std::string>>& filterLayerIDs,
    const GeometryTile& geometryTile,
    const Style& style,
    const float bearing,
    const float pixelsToTileUnits) const {

    auto& layerIDs = bucketLayerIDs.at(indexedFeature.bucketName);

    if (filterLayerIDs && !vectorsIntersect(layerIDs, *filterLayerIDs)) return;

    auto sourceLayer = geometryTile.getLayer(indexedFeature.sourceLayerName);
    assert(sourceLayer);
    auto feature = sourceLayer->getFeature(indexedFeature.index);
    assert(feature);

    for (auto& layerID : layerIDs) {

        if (filterLayerIDs && !vectorContains(*filterLayerIDs, layerID)) continue;

        auto styleLayer = style.getLayer(layerID);
        if (!styleLayer) continue;

        if (!styleLayer->is<SymbolLayer>()) {
            auto geometries = getGeometries(*feature);
            if (!styleLayer->queryIntersectsGeometry(queryGeometry, geometries, bearing, pixelsToTileUnits)) continue;
        }

        auto& layerResult = result[layerID];

        auto properties = feature->getProperties();
        rapidjson::StringBuffer buffer;
        buffer.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        writer.Key("type");
        writer.String("Feature");
        auto id = feature->getID();
        if (id) {
            writer.Key("id");
            writer.Double(feature->getID());
        }
        writer.Key("properties");
        writer.StartObject();
        for (auto& prop : properties) {
            std::string key = prop.first;
            Value& value = prop.second;

            writer.Key(key.c_str());

            if (value.is<std::string>()) {
                writer.String(value.get<std::string>().c_str());
            } else if (value.is<bool>()) {
                writer.Bool(value.get<bool>());
            } else if (value.is<int64_t>()) {
                writer.Int64(value.get<int64_t>());
            } else if (value.is<uint64_t>()) {
                writer.Uint64(value.get<uint64_t>());
            } else if (value.is<double>()) {
                writer.Double(value.get<double>());
            }
        }
        writer.EndObject();
        writer.EndObject();

        layerResult.push_back(buffer.GetString());
    }
}

optional<GeometryCollection> FeatureIndex::translateQueryGeometry(
        const GeometryCollection& queryGeometry,
        const std::array<float, 2>& translate,
        const TranslateAnchorType anchorType,
        const float bearing,
        const float pixelsToTileUnits) {

    if (translate[0] == 0 && translate[1] == 0) return {};

    GeometryCoordinate translateVec(translate[0] * pixelsToTileUnits, translate[1] * pixelsToTileUnits);

    if (anchorType == TranslateAnchorType::Viewport) {
        translateVec = util::rotate(translateVec, -bearing);
    }

    GeometryCollection translated;
    for (auto& ring : queryGeometry) {
        translated.emplace_back();
        auto& translatedRing = translated.back();
        for (auto& p : ring) {
            translatedRing.push_back(p - translateVec);
        }
    }
    return translated;
}

void FeatureIndex::addBucketLayerName(const std::string& bucketName, const std::string& layerID) {
    auto& layerIDs = bucketLayerIDs[bucketName];
    layerIDs.push_back(layerID);
}

void FeatureIndex::setCollisionTile(std::unique_ptr<CollisionTile> collisionTile_) {
    collisionTile = std::move(collisionTile_);
}
