#pragma once
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>

namespace mbgl {

class CircleLayout final : public Layout {
public:
    CircleLayout(const BucketParameters& parameters,
                 const std::vector<Immutable<style::LayerProperties>>& group,
                 std::unique_ptr<GeometryTileLayer> sourceLayer_)
        : sourceLayer(std::move(sourceLayer_)), zoom(parameters.tileID.overscaledZ), mode(parameters.mode) {
        assert(!group.empty());
        auto leaderLayerProperties = staticImmutableCast<style::CircleLayerProperties>(group.front());
        const auto& unevaluatedLayout = leaderLayerProperties->layerImpl().layout;
        const bool sortFeaturesByKey = !unevaluatedLayout.get<style::CircleSortKey>().isUndefined();
        const auto& layout = unevaluatedLayout.evaluate(PropertyEvaluationParameters(zoom));
        sourceLayerID = leaderLayerProperties->layerImpl().sourceLayer;
        bucketLeaderID = leaderLayerProperties->layerImpl().id;

        for (const auto& layerProperties : group) {
            const std::string& layerId = layerProperties->baseImpl->id;
            layerPropertiesMap.emplace(layerId, layerProperties);
        }

        const size_t featureCount = sourceLayer->featureCount();
        for (size_t i = 0; i < featureCount; ++i) {
            auto feature = sourceLayer->getFeature(i);
            if (!leaderLayerProperties->layerImpl().filter(style::expression::EvaluationContext(zoom, feature.get())
                                                               .withCanonicalTileID(&parameters.tileID.canonical))) {
                continue;
            }

            if (!sortFeaturesByKey) {
                features.push_back({i, std::move(feature), style::CircleSortKey::defaultValue()});
                continue;
            }

            const auto& sortKeyProperty = layout.template get<style::CircleSortKey>();
            float sortKey = sortKeyProperty.evaluate(*feature, zoom, style::CircleSortKey::defaultValue());
            CircleFeature circleFeature{i, std::move(feature), sortKey};
            const auto sortPosition = std::lower_bound(features.cbegin(), features.cend(), circleFeature);
            features.insert(sortPosition, std::move(circleFeature));
        }
    }

    bool hasDependencies() const override { return false; }

    void createBucket(const ImagePositions&,
                      std::unique_ptr<FeatureIndex>& featureIndex,
                      std::unordered_map<std::string, LayerRenderData>& renderData,
                      const bool,
                      const bool,
                      const CanonicalTileID& canonical) override {
        auto bucket = std::make_shared<CircleBucket>(layerPropertiesMap, mode, zoom);

        for (auto& circleFeature : features) {
            const auto i = circleFeature.i;
            const std::unique_ptr<GeometryTileFeature>& feature = circleFeature.feature;
            const GeometryCollection& geometries = feature->getGeometries();

            addCircle(*bucket, *feature, geometries, i, circleFeature.sortKey, canonical);

            bucket->addFeature(*feature, geometries, {}, PatternLayerMap(), i, canonical);
            featureIndex->insert(geometries, i, sourceLayerID, bucketLeaderID);
        }

        if (!bucket->hasData()) return;

        for (const auto& pair : layerPropertiesMap) {
            renderData.emplace(pair.first, LayerRenderData{bucket, pair.second});
        }
    }

private:
    struct CircleFeature {
        friend bool operator<(const CircleFeature& lhs, const CircleFeature& rhs) { return lhs.sortKey < rhs.sortKey; }

        size_t i;
        std::unique_ptr<GeometryTileFeature> feature;
        float sortKey;
    };

    void addCircle(CircleBucket& bucket,
                   const GeometryTileFeature& feature,
                   const GeometryCollection& geometry,
                   std::size_t featureIndex,
                   float sortKey,
                   const CanonicalTileID& canonical) {
        constexpr const uint16_t vertexLength = 4;

        auto& segments = bucket.segments;
        auto& vertices = bucket.vertices;
        auto& triangles = bucket.triangles;

        for (auto& circle : geometry) {
            for (auto& point : circle) {
                auto x = point.x;
                auto y = point.y;

                // Do not include points that are outside the tile boundaries.
                // Include all points in Still mode. You need to include points from
                // neighbouring tiles so that they are not clipped at tile boundaries.
                if ((mode == MapMode::Continuous) && (x < 0 || x >= util::EXTENT || y < 0 || y >= util::EXTENT))
                    continue;

                if (segments.empty() ||
                    segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
                    // Move to a new segments because the old one can't hold the geometry.
                    segments.emplace_back(vertices.elements(), triangles.elements(), 0ul, 0ul, sortKey);
                }

                // this geometry will be of the Point type, and we'll derive
                // two triangles from it.
                //
                // ┌─────────┐
                // │ 4     3 │
                // │         │
                // │ 1     2 │
                // └─────────┘
                //
                vertices.emplace_back(CircleProgram::vertex(point, -1, -1)); // 1
                vertices.emplace_back(CircleProgram::vertex(point, 1, -1));  // 2
                vertices.emplace_back(CircleProgram::vertex(point, 1, 1));   // 3
                vertices.emplace_back(CircleProgram::vertex(point, -1, 1));  // 4

                auto& segment = segments.back();
                assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
                uint16_t index = segment.vertexLength;

                // 1, 2, 3
                // 1, 4, 3
                triangles.emplace_back(index, index + 1, index + 2);
                triangles.emplace_back(index, index + 3, index + 2);

                segment.vertexLength += vertexLength;
                segment.indexLength += 6;
            }
        }

        for (auto& pair : bucket.paintPropertyBinders) {
            pair.second.populateVertexVectors(feature, vertices.elements(), featureIndex, {}, {}, canonical);
        }
    }

    std::map<std::string, Immutable<style::LayerProperties>> layerPropertiesMap;
    std::string bucketLeaderID;

    const std::unique_ptr<GeometryTileLayer> sourceLayer;
    std::list<CircleFeature> features;

    const float zoom;
    const MapMode mode;
    std::string sourceLayerID;
};

} // namespace mbgl
