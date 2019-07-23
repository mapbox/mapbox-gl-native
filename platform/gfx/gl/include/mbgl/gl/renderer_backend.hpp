#pragma once

#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/util.hpp>

namespace mbgl {
namespace gl {

class Context;
using ProcAddress = void (*)();
using FramebufferID = uint32_t;

class RendererBackend : public gfx::RendererBackend {
public:
    RendererBackend(gfx::ContextMode);
    ~RendererBackend() override;

    // Called prior to rendering to update the internally assumed OpenGL state.
    virtual void updateAssumedState() = 0;

protected:
    std::unique_ptr<gfx::Context> createContext() override;

    // Called with the name of an OpenGL extension that should be loaded. RendererBackend implementations
    // must call the API-specific version that obtains the function pointer for this function,
    // or a null pointer if unsupported/unavailable.
    virtual ProcAddress getExtensionFunctionPointer(const char*) = 0;

    // Reads the color pixel data from the currently bound framebuffer.
    PremultipliedImage readFramebuffer(const Size&);

    // A constant to signal that a framebuffer is bound, but with an unknown ID.
    static constexpr const FramebufferID ImplicitFramebufferBinding =
        std::numeric_limits<FramebufferID>::max();

    // Tells the renderer that OpenGL state has already been set by the windowing toolkit.
    // It sets the internal assumed state to the supplied values.
    void assumeFramebufferBinding(FramebufferID fbo);
    void assumeViewport(int32_t x, int32_t y, const Size&);
    void assumeScissorTest(bool);

    // Returns true when assumed framebuffer binding hasn't changed from the implicit binding.
    bool implicitFramebufferBound();

public:
    // Triggers an OpenGL state update if the internal assumed state doesn't match the
    // supplied values.
    void setFramebufferBinding(FramebufferID fbo);
    void setViewport(int32_t x, int32_t y, const Size&);
    void setScissorTest(bool);
};

} // namespace gl
} // namespace mbgl
