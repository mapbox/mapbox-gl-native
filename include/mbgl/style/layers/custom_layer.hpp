#pragma once

#include <mbgl/style/layer.hpp>

#include <array>

namespace mbgl {
namespace style {

/**
 * Parameters that define the current camera position for a `CustomLayerHost::render()` function.
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
    std::array<double, 16> projectionMatrix;
};

class CustomLayerHost {
public:
    virtual ~CustomLayerHost() = default;
    /**
     * Initialize any GL state needed by the custom layer. This method is called once, from the
     * main thread, at a point when the GL context is active but before rendering for the first
     * time.
     *
     * Resources that are acquired in this method must be released in the `deinitialize` function.
     */
    virtual void initialize() = 0;

    /**
     * Render the layer. This method is called once per frame. The implementation should not make
     * any assumptions about the GL state (other than that the correct context is active). It may
     * make changes to the state, and is not required to reset values such as the depth mask, stencil
     * mask, and corresponding test flags to their original values.
     * Make sure that you are drawing your fragments with a z value of 1 to take advantage of the
     * opaque fragment culling in case there are opaque layers above your custom layer.
     */
    virtual void render(const CustomLayerRenderParameters&) = 0;

    /**
     * Called when the system has destroyed the underlying GL context. The
     * `deinitialize` function will not be called in this case, however
     * `initialize` will be called instead to prepare for a new render.
     *
     */
    virtual void contextLost() = 0;

    /**
     * Destroy any GL state needed by the custom layer, and deallocate context, if necessary. This
     * method is called once, from the main thread, at a point when the GL context is active.
     *
     * Note that it may be called even when the `initialize` function has not been called.
     */
    virtual void deinitialize() = 0;
};

class CustomLayer : public Layer {
public:
    CustomLayer(const std::string& id,
                std::unique_ptr<CustomLayerHost> host);

    ~CustomLayer() final;

    // Dynamic properties
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

    CustomLayer(const CustomLayer&) = delete;

    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

class CustomLayerFactory : public LayerFactory {
public:
    CustomLayerFactory();
    // LayerFactory overrides.
    ~CustomLayerFactory() override;
    bool supportsType(const std::string& type) const noexcept final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const conversion::Convertible& value) final;

    static CustomLayerFactory* get() noexcept;

private:
    static CustomLayerFactory* instance;
};

} // namespace style
} // namespace mbgl
