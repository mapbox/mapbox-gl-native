#pragma once

#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/util.hpp>

#include <memory>
#include <mutex>

namespace mbgl {

namespace gl {
class Context;
using ProcAddress = void (*)();
using FramebufferID = uint32_t;
} // namespace gl

// The RendererBackend is used by the Renderer to facilitate
// the actual rendering.
class RendererBackend {
public:
    RendererBackend();
    virtual ~RendererBackend();

    // Returns the backend's context which manages OpenGL state.
    gl::Context& getContext();

    // Called prior to rendering to update the internally assumed OpenGL state.
    virtual void updateAssumedState() = 0;

    // Called when this backend is used for rendering. Implementations should ensure that a renderable
    // object is bound and glClear/glDraw* calls can be done. They should also make sure that
    // calling .bind() repeatedly is a no-op and that the appropriate gl::Context values are
    // set to the current state.
    virtual void bind() = 0;

    virtual Size getFramebufferSize() const = 0;

protected:
    // Called with the name of an OpenGL extension that should be loaded. RendererBackend implementations
    // must call the API-specific version that obtains the function pointer for this function,
    // or a null pointer if unsupported/unavailable.
    virtual gl::ProcAddress getExtensionFunctionPointer(const char*) = 0;

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

    // Reads the color pixel data from the currently bound framebuffer.
    PremultipliedImage readFramebuffer(const Size&) const;

    // A constant to signal that a framebuffer is bound, but with an unknown ID.
    static constexpr const gl::FramebufferID ImplicitFramebufferBinding =
        std::numeric_limits<gl::FramebufferID>::max();

    // Tells the renderer that OpenGL state has already been set by the windowing toolkit.
    // It sets the internal assumed state to the supplied values.
    void assumeFramebufferBinding(gl::FramebufferID fbo);
    void assumeViewport(int32_t x, int32_t y, const Size&);
    void assumeScissorTest(bool);

    // Returns true when assumed framebuffer binding hasn't changed from the implicit binding.
    bool implicitFramebufferBound();

    // Triggers an OpenGL state update if the internal assumed state doesn't match the
    // supplied values.
    void setFramebufferBinding(gl::FramebufferID fbo);
    void setViewport(int32_t x, int32_t y, const Size&);
    void setScissorTest(bool);

protected:
    std::unique_ptr<gl::Context> context;

private:
    std::once_flag initialized;

    friend class BackendScope;
};

MBGL_CONSTEXPR bool operator==(const RendererBackend& a, const RendererBackend& b) {
    return &a == &b;
}

} // namespace mbgl
