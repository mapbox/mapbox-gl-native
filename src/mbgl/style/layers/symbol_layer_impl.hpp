#pragma once

#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>

namespace mbgl {

class SpriteAtlas;
class SymbolLayout;

namespace style {

// Repackaging evaluated values from SymbolLayoutProperties + SymbolPaintProperties
// for genericity over icons vs. text.
class SymbolPropertyValues {
public:
    // Layout
    AlignmentType pitchAlignment;
    AlignmentType rotationAlignment;
    float layoutSize;

    // Paint
    float opacity;
    Color color;
    Color haloColor;
    float haloWidth;
    float haloBlur;
    std::array<float, 2> translate;
    TranslateAnchorType translateAnchor;
    float paintSize;

    float sdfScale;   // Constant (1.0 or 24.0)

    bool hasHalo() const {
        return haloColor.a > 0.0f && haloWidth > 0.0f;
    }

    bool hasForeground() const {
        return color.a > 0.0f;
    }
};

class SymbolLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const override;

    void cascade(const CascadeParameters&) override;
    bool evaluate(const PropertyEvaluationParameters&) override;

    std::unique_ptr<Bucket> createBucket(BucketParameters&) const override;
    std::unique_ptr<SymbolLayout> createLayout(BucketParameters&) const;

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
