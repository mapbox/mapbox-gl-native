#ifndef MBGL_CUSTOM_LAYER
#define MBGL_CUSTOM_LAYER

#include <mbgl/style/style_layer.hpp>

namespace mbgl {

class TransformState;

class CustomLayer : public StyleLayer {
public:
    CustomLayer(const std::string& id,
                CustomLayerInitializeFunction,
                CustomLayerRenderFunction,
                CustomLayerDeinitializeFunction,
                void* context);

    CustomLayer(const CustomLayer&);
    ~CustomLayer();

    void initialize();
    void render(const TransformState&) const;

private:
    std::unique_ptr<StyleLayer> clone() const final;

    void parseLayout(const JSValue&) final {}
    void parsePaints(const JSValue&) final {}

    void cascade(const StyleCascadeParameters&) final {}
    bool recalculate(const StyleCalculationParameters&) final;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const final;

    CustomLayerInitializeFunction initializeFn = nullptr;
    CustomLayerRenderFunction renderFn = nullptr;
    CustomLayerDeinitializeFunction deinitializeFn = nullptr;
    void* context = nullptr;
};

} // namespace mbgl

#endif
