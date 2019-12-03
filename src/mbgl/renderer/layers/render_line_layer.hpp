#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/layout/pattern_layout.hpp>
#include <mbgl/gfx/texture.hpp>

namespace mbgl {

class RenderLineLayer final : public RenderLayer {
public:
    explicit RenderLineLayer(Immutable<style::LineLayer::Impl>);
    ~RenderLineLayer() override;

private:
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void prepare(const LayerPrepareParameters&) override;
    void upload(gfx::UploadPass&) override;
    void render(PaintParameters&) override;

    bool queryIntersectsFeature(const GeometryCoordinates&, const GeometryTileFeature&, const float,
                                const TransformState&, const float, const mat4&, const FeatureState&) const override;

    // Paint properties
    style::LinePaintProperties::Unevaluated unevaluated;

    float getLineWidth(const GeometryTileFeature&, const float, const FeatureState&) const;
    void updateColorRamp();

    PremultipliedImage colorRamp;
    optional<gfx::Texture> colorRampTexture;
};

} // namespace mbgl
