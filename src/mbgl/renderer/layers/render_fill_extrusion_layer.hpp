#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>
#include <mbgl/gfx/offscreen_texture.hpp>

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

    bool queryIntersectsFeature(
        const GeometryCoordinates&,
        const GeometryTileFeature&,
        const float,
        const TransformState&,
        const float,
        const mat4&) const override;

    // Paint properties
    style::FillExtrusionPaintProperties::Unevaluated unevaluated;
    style::FillExtrusionPaintProperties::PossiblyEvaluated evaluated;

    const style::FillExtrusionLayer::Impl& impl() const;

    std::unique_ptr<gfx::OffscreenTexture> renderTexture;
private:
    CrossfadeParameters crossfade;
};

inline const RenderFillExtrusionLayer* toRenderFillExtrusionLayer(const RenderLayer* layer) {
    return static_cast<const RenderFillExtrusionLayer*>(layer);
}

} // namespace mbgl
