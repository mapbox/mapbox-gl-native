#pragma once
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layer_properties.hpp>

namespace mbgl {

class PatternDependency {
public:
    std::string min;
    std::string mid;
    std::string max;
};

using PatternLayerMap = std::map<std::string, PatternDependency>;

class PatternFeature  {
public:
    const uint32_t i;
    std::unique_ptr<GeometryTileFeature> feature;
    PatternLayerMap patterns;
};

template <class BucketType,
          class LayerPropertiesType,
          class PatternPropertyType,
          class PossiblyEvaluatedLayoutPropertiesType = typename style::Properties<>::PossiblyEvaluated>
class PatternLayout : public Layout {
public:
    PatternLayout(const BucketParameters& parameters,
                  const std::vector<Immutable<style::LayerProperties>>& group,
                  std::unique_ptr<GeometryTileLayer> sourceLayer_,
                  ImageDependencies& patternDependencies)
                  : sourceLayer(std::move(sourceLayer_)),
                    zoom(parameters.tileID.overscaledZ),
                    overscaling(parameters.tileID.overscaleFactor()),
                    hasPattern(false) {
        assert(!group.empty());
        auto leaderLayerProperties = staticImmutableCast<LayerPropertiesType>(group.front());
        layout = leaderLayerProperties->layerImpl().layout.evaluate(PropertyEvaluationParameters(zoom));
        sourceLayerID = leaderLayerProperties->layerImpl().sourceLayer;
        bucketLeaderID = leaderLayerProperties->layerImpl().id;

        for (const auto& layerProperties : group) {
            const std::string& layerId = layerProperties->baseImpl->id;
            const auto& evaluated = style::getEvaluated<LayerPropertiesType>(layerProperties);
            const auto patternProperty = evaluated.template get<PatternPropertyType>();
            const auto constantPattern = patternProperty.constantOr(Faded<std::basic_string<char> >{ "", ""});
            // determine if layer group has any layers that use *-pattern property and add
            // constant pattern dependencies.
            if (!patternProperty.isConstant()) {
                hasPattern = true;
            } else if (!constantPattern.to.empty()){
                hasPattern = true;
                patternDependencies.emplace(constantPattern.to, ImageType::Pattern);
                patternDependencies.emplace(constantPattern.from, ImageType::Pattern);
            }
            layerPropertiesMap.emplace(layerId, layerProperties);
        }

        const size_t featureCount = sourceLayer->featureCount();
        for (size_t i = 0; i < featureCount; ++i) {
            auto feature = sourceLayer->getFeature(i);
            if (!leaderLayerProperties->layerImpl().filter(style::expression::EvaluationContext { this->zoom, feature.get() }))
                continue;

            PatternLayerMap patternDependencyMap;
            if (hasPattern) {
                for (const auto& layerProperties : group) {
                    const std::string& layerId = layerProperties->baseImpl->id;
                    const auto it = layerPropertiesMap.find(layerId);
                    if (it != layerPropertiesMap.end()) {
                        const auto paint = static_cast<const LayerPropertiesType&>(*it->second).evaluated;
                        const auto patternProperty = paint.template get<PatternPropertyType>();
                        if (!patternProperty.isConstant()) {
                            // For layers with non-data-constant pattern properties, evaluate their expression and add
                            // the patterns to the dependency vector
                            const auto min = patternProperty.evaluate(*feature, zoom - 1, PatternPropertyType::defaultValue());
                            const auto mid = patternProperty.evaluate(*feature, zoom, PatternPropertyType::defaultValue());
                            const auto max = patternProperty.evaluate(*feature, zoom + 1, PatternPropertyType::defaultValue());

                            patternDependencies.emplace(min.to, ImageType::Pattern);
                            patternDependencies.emplace(mid.to, ImageType::Pattern);
                            patternDependencies.emplace(max.to, ImageType::Pattern);
                            patternDependencyMap.emplace(layerId, PatternDependency {min.to, mid.to, max.to});

                        }
                    }
                }
            }
            features.push_back({static_cast<uint32_t>(i), std::move(feature), patternDependencyMap});
        }
    };

    ~PatternLayout() final = default;

    bool hasDependencies() const override {
        return hasPattern;
    }

    void createBucket(const ImagePositions& patternPositions, std::unique_ptr<FeatureIndex>& featureIndex, std::unordered_map<std::string, LayerRenderData>& renderData, const bool, const bool) override {
        auto bucket = std::make_shared<BucketType>(layout, layerPropertiesMap, zoom, overscaling);
        for (auto & patternFeature : features) {
            const auto i = patternFeature.i;
            std::unique_ptr<GeometryTileFeature> feature = std::move(patternFeature.feature);
            const PatternLayerMap& patterns = patternFeature.patterns;
            const GeometryCollection& geometries = feature->getGeometries();

            bucket->addFeature(*feature, geometries, patternPositions, patterns);
            featureIndex->insert(geometries, i, sourceLayerID, bucketLeaderID);
        }
        if (bucket->hasData()) {
            for (const auto& pair : layerPropertiesMap) {
                renderData.emplace(pair.first, LayerRenderData {bucket, pair.second});
            }
        }
    };

private:
    std::map<std::string, Immutable<style::LayerProperties>> layerPropertiesMap;
    std::string bucketLeaderID;

    const std::unique_ptr<GeometryTileLayer> sourceLayer;
    std::vector<PatternFeature> features;
    PossiblyEvaluatedLayoutPropertiesType layout;

    const float zoom;
    const uint32_t overscaling;
    std::string sourceLayerID;
    bool hasPattern;
};

} // namespace mbgl

