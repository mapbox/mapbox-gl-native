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
};

enum class SymbolType : uint8_t {
    Text,
    IconRGBA,
    IconSDF
};

} // namespace style

class RenderSymbolLayer final: public RenderLayer {
public:
    explicit RenderSymbolLayer(Immutable<style::SymbolLayer::Impl>);
    ~RenderSymbolLayer() override;

    static style::IconPaintProperties::PossiblyEvaluated iconPaintProperties(const style::SymbolPaintProperties::PossiblyEvaluated&);
    static style::TextPaintProperties::PossiblyEvaluated textPaintProperties(const style::SymbolPaintProperties::PossiblyEvaluated&);

private:
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&) override;
    void prepare(const LayerPrepareParameters&) override;

    // Paint properties
    style::SymbolPaintProperties::Unevaluated unevaluated;

    float iconSize = 1.0f;
    float textSize = 16.0f;

    bool hasFormatSectionOverrides = false;
};

} // namespace mbgl
