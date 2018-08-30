#pragma once
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>

namespace mbgl {

class CircleFeature  {
public:
    const uint32_t i;
    std::unique_ptr<GeometryTileFeature> feature;
};

template <class B>
class CircleLayout : public Layout {
public:

    CircleLayout(const BucketParameters& parameters,
                 const std::vector<const RenderLayer*>& layers,
                 std::unique_ptr<GeometryTileLayer> sourceLayer_,
                 ImageDependencies&)
                : Layout(),
                bucketLeaderID(layers.at(0)->getID()),
                mode(parameters.mode),
                sourceLayer(std::move(sourceLayer_)),
                zoom(parameters.tileID.overscaledZ),
                overscaling(parameters.tileID.overscaleFactor()) {

        using Layer = typename B::RenderLayerType;
        const auto renderLayer = static_cast<const Layer*>(layers.at(0));
        const typename Layer::StyleLayerImpl& leader = renderLayer->impl();
        sourceLayerID = leader.sourceLayer;

        for (const auto& layer : layers) {
            const typename B::PossiblyEvaluatedPaintProperties evaluatedProps = static_cast<const Layer*>(layer)->paintProperties();
            layerPaintProperties.emplace(layer->getID(), std::move(evaluatedProps));
        }

        const size_t featureCount = sourceLayer->featureCount();
        for (size_t i = 0; i < featureCount; ++i) {
            auto feature = sourceLayer->getFeature(i);
            if (!leader.filter(style::expression::EvaluationContext { this->zoom, feature.get() })) continue;
            features.push_back({static_cast<uint32_t>(i), std::move(feature)});
        }
    }

    ~CircleLayout() final = default;

    bool hasDependencies() const override {
        return false;
    }

    void createBucket(const ImagePositions&, std::unique_ptr<FeatureIndex>& featureIndex, std::unordered_map<std::string, std::shared_ptr<Bucket>>& buckets, const bool, const bool) override {
        auto bucket = std::make_shared<B>(layerPaintProperties, zoom, mode);
        for (auto & f : features) {
            const auto i = f.i;
            std::unique_ptr<GeometryTileFeature> feature = std::move(f.feature);
            GeometryCollection geometries = feature->getGeometries();

            bucket->addFeature(*feature, geometries, {}, {});
            featureIndex->insert(geometries, i, sourceLayerID, bucketLeaderID);
        }

        if (bucket->hasData()) {
            for (const auto& pair : layerPaintProperties) {
                buckets.emplace(pair.first, bucket);
            }
        }
    };

    std::map<std::string, typename B::PossiblyEvaluatedPaintProperties> layerPaintProperties;
    std::string bucketLeaderID;
    const MapMode mode;
private:
    const std::unique_ptr<GeometryTileLayer> sourceLayer;
    std::vector<CircleFeature> features;

    std::string sourceLayerID;
    const float zoom;
    const uint32_t overscaling;
};

} // namespace mbgl
