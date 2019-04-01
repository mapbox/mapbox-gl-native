#pragma once

#include <mbgl/util/util.hpp>

#include <memory>
#include <mutex>

namespace mbgl {
namespace gfx {

class Context;
class Renderable;
class BackendScope;

// TODO: Rename to "Device"
class RendererBackend {
protected:
    explicit RendererBackend();

public:
    virtual ~RendererBackend();
    RendererBackend(const RendererBackend&) = delete;
    RendererBackend& operator=(const RendererBackend&) = delete;

    // Returns the device's context.
    Context& getContext();

    // Returns a reference to the default surface that should be rendered on.
    virtual Renderable& getDefaultRenderable() = 0;

protected:
    virtual std::unique_ptr<Context> createContext() = 0;

    // Called when the backend's GL context needs to be made active or inactive. These are called,
    // as a matched pair, exclusively through BackendScope, in two situations:
    //
    //   1. When releasing GL resources during Renderer destruction
    //      (Including calling CustomLayerHost::deinitialize during RenderCustomLayer destruction)
    //   2. When renderering through Renderer::render()
    //      (Including calling CustomLayerHost::initialize for newly added custom layers and
    //       CustomLayerHost::deinitialize on layer removal)
    virtual void activate() = 0;
    virtual void deactivate() = 0;

protected:
    std::unique_ptr<Context> context;
    std::once_flag initialized;

    friend class BackendScope;
};

MBGL_CONSTEXPR bool operator==(const RendererBackend& a, const RendererBackend& b) {
    return &a == &b;
}

} // namespace gfx
} // namespace mbgl
