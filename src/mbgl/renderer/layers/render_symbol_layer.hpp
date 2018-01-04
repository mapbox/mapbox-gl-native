#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/image_impl.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

namespace style {

// {icon,text}-specific paint-property packs for use in the symbol Programs.
// Since each program deals either with icons or text, using a smaller property set
// lets us avoid unnecessarily binding attributes for properties the program wouldn't use.
class IconPaintProperties : public Properties<
        IconOpacity,
        IconColor,
        IconHaloColor,
        IconHaloWidth,
        IconHaloBlur,
        IconTranslate,
        IconTranslateAnchor
> {};

class TextPaintProperties : public Properties<
        TextOpacity,
        TextColor,
        TextHaloColor,
        TextHaloWidth,
        TextHaloBlur,
        TextTranslate,
        TextTranslateAnchor
> {};

// Repackaging evaluated values from SymbolLayoutProperties + SymbolPaintProperties
// for genericity over icons vs. text.
class SymbolPropertyValues {
public:
    // Layout
    AlignmentType pitchAlignment;
    AlignmentType rotationAlignment;
    bool keepUpright;

    // Paint
    std::array<float, 2> translate;
    TranslateAnchorType translateAnchor;

    bool hasHalo;
    bool hasFill;
    
    float maxCameraDistance; // 1.5 for road labels, or 10 (essentially infinite) for everything else
};

} // namespace style

class BucketParameters;
class SymbolLayout;
class GeometryTileLayer;

class RenderSymbolLayer: public RenderLayer {
public:
    RenderSymbolLayer(Immutable<style::SymbolLayer::Impl>);
    ~RenderSymbolLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    void render(PaintParameters&, RenderSource*) override;

    style::IconPaintProperties::PossiblyEvaluated iconPaintProperties() const;
    style::TextPaintProperties::PossiblyEvaluated textPaintProperties() const;

    style::SymbolPropertyValues iconPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated&) const;
    style::SymbolPropertyValues textPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated&) const;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;
    std::unique_ptr<SymbolLayout> createLayout(const BucketParameters&,
                                               const std::vector<const RenderLayer*>&,
                                               std::unique_ptr<GeometryTileLayer>,
                                               GlyphDependencies&,
                                               ImageDependencies&) const;

    // Paint properties
    style::SymbolPaintProperties::Unevaluated unevaluated;
    style::SymbolPaintProperties::PossiblyEvaluated evaluated;

    float iconSize = 1.0f;
    float textSize = 16.0f;

    const style::SymbolLayer::Impl& impl() const;
};

template <>
inline bool RenderLayer::is<RenderSymbolLayer>() const {
    return type == style::LayerType::Symbol;
}

} // namespace mbgl
