#ifndef MBGL_CUSTOM_LAYER
#define MBGL_CUSTOM_LAYER

#include <mbgl/style/style_layer.hpp>

namespace mbgl {

class StyleRenderParameters;

class CustomLayer : public StyleLayer {
public:
    CustomLayer(const std::string& id);
    CustomLayer(const CustomLayer&) = default;

    /**
     * Initialize any GL state needed by the custom layer. This method is called once, from the
     * rendering thread, at a point when the GL context is active but before rendering for the
     * first time.
     *
     * Resources that are acquired in this method must be released in the subclass's destructor.
     * However, note that in some situations the destructor may be called without initialize()
     * having been called. The destructor should be prepared for this case, e.g. by checking if
     * the resources were actually acquired before releasing them.
     */
    virtual void initialize() = 0;

    /**
     * Render the layer. This method is called once per frame. The implementation should not make
     * any assumptions about the GL state (other than that the correct context is active). It may
     * make changes to the state, and is not required to reset values such as the depth mask, stencil
     * mask, and corresponding test flags to their original values.
     *
     * The input StyleRenderParameters object has a `TransformState state` member that can be queried
     * for information about the current map view.
     */
    virtual void render(const StyleRenderParameters&) const = 0;

    void parseLayout(const JSVal&) final {}
    void parsePaints(const JSVal&) final {}

    void cascade(const StyleCascadeParameters&) final {}
    bool recalculate(const StyleCalculationParameters&) final;

    std::unique_ptr<Bucket> createBucket(StyleBucketParameters&) const final;
};

} // namespace mbgl

#endif
