#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>
#include <mbgl/layout/pattern_layout.hpp>

namespace mbgl {

class FillBucket;

class RenderFillLayer: public RenderLayer {
public:
    using StyleLayerImpl = style::FillLayer::Impl;
    using PatternProperty = style::FillPattern;

    RenderFillLayer(Immutable<style::FillLayer::Impl>);
    ~RenderFillLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&, RenderSource*) override;
    style::FillPaintProperties::PossiblyEvaluated paintProperties() const;

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
    style::FillPaintProperties::Unevaluated unevaluated;
    style::FillPaintProperties::PossiblyEvaluated evaluated;

    const style::FillLayer::Impl& impl() const;
private:
    CrossfadeParameters crossfade;

};

template <>
inline bool RenderLayer::is<RenderFillLayer>() const {
    return type == style::LayerType::Fill;
}

} // namespace mbgl
