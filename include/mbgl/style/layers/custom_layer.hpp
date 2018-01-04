#pragma once

#include <mbgl/style/layer.hpp>

namespace mbgl {
namespace style {

/**
 * Initialize any GL state needed by the custom layer. This method is called once, from the
 * main thread, at a point when the GL context is active but before rendering for the first
 * time.
 *
 * Resources that are acquired in this method must be released in the UninitializeFunction.
 */
using CustomLayerInitializeFunction = void (*)(void* context);

/**
 * Parameters that define the current camera position for a CustomLayerRenderFunction.
 */
struct CustomLayerRenderParameters {
    double width;
    double height;
    double latitude;
    double longitude;
    double zoom;
    double bearing;
    double pitch;
    double fieldOfView;
};

/**
 * Render the layer. This method is called once per frame. The implementation should not make
 * any assumptions about the GL state (other than that the correct context is active). It may
 * make changes to the state, and is not required to reset values such as the depth mask, stencil
 * mask, and corresponding test flags to their original values.
 * Make sure that you are drawing your fragments with a z value of 1 to take advantage of the
 * opaque fragment culling in case there are opaque layers above your custom layer.
 */
using CustomLayerRenderFunction = void (*)(void* context, const CustomLayerRenderParameters&);

/**
 * Called when the system has destroyed the underlying GL context. The
 * `CustomLayerDeinitializeFunction` will not be called in this case, however
 * `CustomLayerInitializeFunction` will be called instead to prepare for a new render.
 *
 */
using CustomLayerContextLostFunction = void (*)(void* context);

/**
 * Destroy any GL state needed by the custom layer, and deallocate context, if necessary. This
 * method is called once, from the main thread, at a point when the GL context is active.
 *
 * Note that it may be called even when the InitializeFunction has not been called.
 */
using CustomLayerDeinitializeFunction = void (*)(void* context);

class CustomLayer : public Layer {
public:
    CustomLayer(const std::string& id,
                CustomLayerInitializeFunction,
                CustomLayerRenderFunction,
                CustomLayerContextLostFunction,
                CustomLayerDeinitializeFunction,
                void* context);

    CustomLayer(const std::string& id,
                CustomLayerInitializeFunction,
                CustomLayerRenderFunction,
                CustomLayerDeinitializeFunction,
                void* context);

    ~CustomLayer() final;

    // Visibility
    void setVisibility(VisibilityType) final;

    // Zoom range
    void setMinZoom(float) final;
    void setMaxZoom(float) final;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

    CustomLayer(const CustomLayer&) = delete;
};

template <>
bool Layer::is<CustomLayer>() const;

} // namespace style
} // namespace mbgl
