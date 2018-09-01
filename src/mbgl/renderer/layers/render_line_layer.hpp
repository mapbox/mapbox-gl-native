#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/layout/pattern_layout.hpp>

namespace mbgl {

struct LineFloorwidth : style::DataDrivenPaintProperty<float, attributes::a_floorwidth, uniforms::u_floorwidth> {
    static float defaultValue() { return 1; }
};

class RenderLinePaintProperties : public style::ConcatenateProperties<
    style::LinePaintProperties,
    style::Properties<LineFloorwidth>> {};

class LineBucket;

class RenderLineLayer: public RenderLayer {
public:
    using StyleLayerImpl = style::LineLayer::Impl;
    using PatternProperty = style::LinePattern;

    RenderLineLayer(Immutable<style::LineLayer::Impl>);
    ~RenderLineLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&, RenderSource*) override;

    RenderLinePaintProperties::PossiblyEvaluated paintProperties() const;

    bool queryIntersectsFeature(
            const GeometryCoordinates&,
            const GeometryTileFeature&,
            const float,
            const TransformState&,
            const float,
            const mat4&) const override;

    void updateColorRamp();

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;
    std::unique_ptr<Layout> createLayout(const BucketParameters&,
                                               const std::vector<const RenderLayer*>&,
                                               std::unique_ptr<GeometryTileLayer>,
                                               GlyphDependencies&,
                                               ImageDependencies&) const override;
    // Paint properties
    style::LinePaintProperties::Unevaluated unevaluated;
    RenderLinePaintProperties::PossiblyEvaluated evaluated;

    const style::LineLayer::Impl& impl() const;

private:
    float getLineWidth(const GeometryTileFeature&, const float) const;
    CrossfadeParameters crossfade;
    PremultipliedImage colorRamp;
    optional<gl::Texture> colorRampTexture;
};

template <>
inline bool RenderLayer::is<RenderLineLayer>() const {
    return type == style::LayerType::Line;
}

} // namespace mbgl
