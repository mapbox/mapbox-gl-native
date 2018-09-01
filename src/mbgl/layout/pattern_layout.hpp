#pragma once
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/renderer/render_layer.hpp>

namespace mbgl {

class PatternDependency {
public:
    std::string min;
    std::string mid;
    std::string max;
};

using PatternLayerMap = std::unordered_map<std::string, PatternDependency>;

class PatternFeature  {
public:
    const uint32_t i;
    std::unique_ptr<GeometryTileFeature> feature;
    PatternLayerMap patterns;
};

template <class B>
class PatternLayout : public Layout {
public:
    PatternLayout(const BucketParameters& parameters,
                  const std::vector<const RenderLayer*>& layers,
                  std::unique_ptr<GeometryTileLayer> sourceLayer_,
                  ImageDependencies& patternDependencies)
                  : Layout(),
                    bucketLeaderID(layers.at(0)->getID()),
                    sourceLayer(std::move(sourceLayer_)),
                    zoom(parameters.tileID.overscaledZ),
                    overscaling(parameters.tileID.overscaleFactor()),
                    hasPattern(false) {

        using PatternLayer = typename B::RenderLayerType;
        const auto renderLayer = layers.at(0)->as<PatternLayer>();
        const typename PatternLayer::StyleLayerImpl& leader = renderLayer->impl();
        layout = leader.layout.evaluate(PropertyEvaluationParameters(zoom));
        sourceLayerID = leader.sourceLayer;
        groupID = renderLayer->getID();

        for (const auto& layer : layers) {
            const typename B::PossiblyEvaluatedPaintProperties evaluatedProps = layer->as<PatternLayer>()->paintProperties();
            layerPaintProperties.emplace(layer->getID(), std::move(evaluatedProps));
            const auto patternProperty = evaluatedProps.template get<typename PatternLayer::PatternProperty>();
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
        }

        const size_t featureCount = sourceLayer->featureCount();
        for (size_t i = 0; i < featureCount; ++i) {
            auto feature = sourceLayer->getFeature(i);
            if (!leader.filter(style::expression::EvaluationContext { this->zoom, feature.get() }))
                continue;

            std::unordered_map<std::string, PatternDependency> patternDependencyMap;
            if (hasPattern) {
                for (const auto& layer : layers) {
                    const auto it = layerPaintProperties.find(layer->getID());
                    if (it != layerPaintProperties.end()) {
                        const auto paint = it->second;
                        const auto patternProperty = paint.template get<typename PatternLayer::PatternProperty>();
                        if (!patternProperty.isConstant()) {
                            // For layers with non-data-constant pattern properties, evaluate their expression and add
                            // the patterns to the dependency vector
                            const auto min = patternProperty.evaluate(*feature, zoom - 1, PatternLayer::PatternProperty::defaultValue());
                            const auto mid = patternProperty.evaluate(*feature, zoom, PatternLayer::PatternProperty::defaultValue());
                            const auto max = patternProperty.evaluate(*feature, zoom + 1, PatternLayer::PatternProperty::defaultValue());

                            patternDependencies.emplace(min.to, ImageType::Pattern);
                            patternDependencies.emplace(mid.to, ImageType::Pattern);
                            patternDependencies.emplace(max.to, ImageType::Pattern);
                            patternDependencyMap.emplace(layer->getID(), PatternDependency {min.to, mid.to, max.to});

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

    void createBucket(const ImagePositions& patternPositions, std::unique_ptr<FeatureIndex>& featureIndex, std::unordered_map<std::string, std::shared_ptr<Bucket>>& buckets, const bool, const bool) override {
        auto bucket = std::make_shared<B>(layout, layerPaintProperties, zoom, overscaling);
        for (auto & patternFeature : features) {
            const auto i = patternFeature.i;
            std::unique_ptr<GeometryTileFeature> feature = std::move(patternFeature.feature);
            PatternLayerMap patterns = patternFeature.patterns;
            GeometryCollection geometries = feature->getGeometries();

            bucket->addFeature(*feature, geometries, patternPositions, patterns);
            featureIndex->insert(geometries, i, sourceLayerID, groupID);
        }
        if (bucket->hasData()) {
            for (const auto& pair : layerPaintProperties) {
                buckets.emplace(pair.first, bucket);
            }
        }
    };

    std::map<std::string, typename B::PossiblyEvaluatedPaintProperties> layerPaintProperties;
    const std::string bucketLeaderID;


private:
    const std::unique_ptr<GeometryTileLayer> sourceLayer;
    std::vector<PatternFeature> features;
    typename B::PossiblyEvaluatedLayoutProperties layout;

    const float zoom;
    const uint32_t overscaling;
    std::string sourceLayerID;
    std::string groupID;
    bool hasPattern;
};

} // namespace mbgl

