#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
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

class PlainIconPaintProperties : public Properties<
        IconOpacity,
        IconTranslate,
        IconTranslateAnchor
> {};

class IconFillPaintProperties : public Properties<
        IconOpacity,
        IconColor,
        IconTranslate,
        IconTranslateAnchor
> {};

class IconHaloPaintProperties : public Properties<
        IconOpacity,
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

class TextFillPaintProperties : public Properties<
        TextOpacity,
        TextColor,
        TextTranslate,
        TextTranslateAnchor
> {};

class TextHaloPaintProperties : public Properties<
        TextOpacity,
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
    PossiblyEvaluatedPropertyValue<float> layoutSize;

    // Paint
    std::array<float, 2> translate;
    TranslateAnchorType translateAnchor;
    float paintSize;

    float sdfScale;   // Constant (1.0 or 24.0)

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

    style::IconPaintProperties::PossiblyEvaluated iconPaintProperties() const;
    style::PlainIconPaintProperties::PossiblyEvaluated plainIconPaintProperties() const;
    style::IconFillPaintProperties::PossiblyEvaluated iconFillPaintProperties() const;
    style::IconHaloPaintProperties::PossiblyEvaluated iconHaloPaintProperties() const;
    style::TextPaintProperties::PossiblyEvaluated textPaintProperties() const;
    style::TextFillPaintProperties::PossiblyEvaluated textFillPaintProperties() const;
    style::TextHaloPaintProperties::PossiblyEvaluated textHaloPaintProperties() const;

    style::SymbolPropertyValues iconPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated&) const;
    style::SymbolPropertyValues textPropertyValues(const style::SymbolLayoutProperties::PossiblyEvaluated&) const;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const override;
    std::unique_ptr<SymbolLayout> createLayout(const BucketParameters&, const std::vector<const RenderLayer*>&,
                                               const GeometryTileLayer&, GlyphDependencies&, IconDependencies&) const;

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
