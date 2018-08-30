#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/offscreen_texture.hpp>

namespace mbgl {

template <class B>
class PatternLayout;

class FillExtrusionBucket;

class RenderFillExtrusionLayer: public RenderLayer {
public:
    using StyleLayerImpl = style::FillExtrusionLayer::Impl;
    using PatternProperty = style::FillExtrusionPattern;

    RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl>);
    ~RenderFillExtrusionLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&, RenderSource*) override;
    style::FillExtrusionPaintProperties::PossiblyEvaluated paintProperties() const;

    bool queryIntersectsFeature(
        const GeometryCoordinates&,
        const GeometryTileFeature&,
        const float,
        const TransformState&,
        const float,
        const mat4&) const override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;
    std::unique_ptr<Layout> createLayout(const BucketParameters&, const std::vector<const RenderLayer*>&,
                                         std::unique_ptr<GeometryTileLayer>, GlyphDependencies&, ImageDependencies&) const override;

    // Paint properties
    style::FillExtrusionPaintProperties::Unevaluated unevaluated;
    style::FillExtrusionPaintProperties::PossiblyEvaluated evaluated;

    const style::FillExtrusionLayer::Impl& impl() const;

    optional<OffscreenTexture> renderTexture;
private:
    CrossfadeParameters crossfade;
};

template <>
inline bool RenderLayer::is<RenderFillExtrusionLayer>() const {
    return type == style::LayerType::FillExtrusion;
}

} // namespace mbgl
