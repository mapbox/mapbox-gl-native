#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

class SpriteAtlas;
class SymbolLayout;

namespace style {
    

// {icon,text}-specific paint-property packs for use in the symbol Programs.
// Since each program deals either with icons or text, using a smaller property set
// lets us avoid unnecessarily binding attributes for properties the program wouldn't use.
class IconPaintProperties : public PaintProperties<
    IconOpacity,
    IconColor,
    IconHaloColor,
    IconHaloWidth,
    IconHaloBlur,
    IconTranslate,
    IconTranslateAnchor
> {};
    
class TextPaintProperties : public PaintProperties<
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
    float layoutSize;

    // Paint
    std::array<float, 2> translate;
    TranslateAnchorType translateAnchor;
    float paintSize;

    float sdfScale;   // Constant (1.0 or 24.0)
    
    bool hasHalo;
    bool hasFill;    
};

class SymbolLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;
    void stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const override;

    void cascade(const CascadeParameters&) override;
    bool evaluate(const PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(const BucketParameters&, const std::vector<const Layer*>&) const override;
    std::unique_ptr<SymbolLayout> createLayout(const BucketParameters&, const std::vector<const Layer*>&,
                                               const GeometryTileLayer&) const;

    IconPaintProperties::Evaluated iconPaintProperties() const;
    TextPaintProperties::Evaluated textPaintProperties() const;
    
    SymbolPropertyValues iconPropertyValues(const SymbolLayoutProperties::Evaluated&) const;
    SymbolPropertyValues textPropertyValues(const SymbolLayoutProperties::Evaluated&) const;

    SymbolLayoutProperties layout;
    SymbolPaintProperties paint;

    float iconSize = 1.0f;
    float textSize = 16.0f;

    SpriteAtlas* spriteAtlas = nullptr;
};

} // namespace style
} // namespace mbgl
