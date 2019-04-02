#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/layers/render_layer_symbol_interface.hpp>
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

} // namespace style

class BucketParameters;
class SymbolLayout;
class GeometryTileLayer;

class RenderSymbolLayer final: public RenderLayer, public RenderLayerSymbolInterface {
public:
    RenderSymbolLayer(Immutable<style::SymbolLayer::Impl>);
    ~RenderSymbolLayer() final = default;

    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    void render(PaintParameters&, RenderSource*) override;
    void setRenderTiles(RenderTiles, const TransformState&) override;

    static style::IconPaintProperties::PossiblyEvaluated iconPaintProperties(const style::SymbolPaintProperties::PossiblyEvaluated&);
    static style::TextPaintProperties::PossiblyEvaluated textPaintProperties(const style::SymbolPaintProperties::PossiblyEvaluated&);

    // RenderLayerSymbolInterface overrides
    const RenderLayerSymbolInterface* getSymbolInterface() const final;
    const std::string& layerID() const final;
    const std::vector<std::reference_wrapper<RenderTile>>& getRenderTiles() const final;
    SymbolBucket* getSymbolBucket(const RenderTile&) const final;

    // Paint properties
    style::SymbolPaintProperties::Unevaluated unevaluated;
    style::SymbolPaintProperties::PossiblyEvaluated evaluated;

    float iconSize = 1.0f;
    float textSize = 16.0f;

    const style::SymbolLayer::Impl& impl() const;

protected:
    static style::SymbolPropertyValues iconPropertyValues(const style::SymbolPaintProperties::PossiblyEvaluated&,
                                                          const style::SymbolLayoutProperties::PossiblyEvaluated&);
    static style::SymbolPropertyValues textPropertyValues(const style::SymbolPaintProperties::PossiblyEvaluated&,
                                                          const style::SymbolLayoutProperties::PossiblyEvaluated&);
    void updateBucketPaintProperties(Bucket*) const final;
};

inline const RenderSymbolLayer* toRenderSymbolLayer(const RenderLayer* layer) {
    return static_cast<const RenderSymbolLayer*>(layer);
}

} // namespace mbgl
