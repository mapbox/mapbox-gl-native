#ifndef MBGL_SYMBOL_LAYER
#define MBGL_SYMBOL_LAYER

#include <mbgl/layer/layer_impl.hpp>
#include <mbgl/layer/symbol_layer.hpp>
#include <mbgl/layer/symbol_layer_properties.hpp>

namespace mbgl {

class SpriteAtlas;

class SymbolLayer::Impl : public Layer::Impl {
public:
    std::unique_ptr<Layer> clone() const override;

    void parseLayout(const JSValue&) override;
    void parsePaints(const JSValue&) override;

    void cascade(const StyleCascadeParameters&) override;
    bool recalculate(const StyleCalculationParameters&) override;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const override;

    SymbolLayoutProperties layout;
    SymbolPaintProperties paint;

    float iconSize = 1.0f;
    float textSize = 16.0f;

    SpriteAtlas* spriteAtlas = nullptr;
};

} // namespace mbgl

#endif
