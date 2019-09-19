#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>
#include <mbgl/layout/pattern_layout.hpp>

namespace mbgl {

class FillBucket;

class RenderFillLayer final : public RenderLayer {
public:
    explicit RenderFillLayer(Immutable<style::FillLayer::Impl>);
    ~RenderFillLayer() override;

private:
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&) override;

    bool queryIntersectsFeature(const GeometryCoordinates&, const GeometryTileFeature&, const float,
                                const TransformState&, const float, const mat4&, const FeatureState&) const override;

    // Paint properties
    style::FillPaintProperties::Unevaluated unevaluated;
};

} // namespace mbgl
