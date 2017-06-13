#pragma once

#include <mbgl/map/view.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/size.hpp>

#include <memory>
#include <mutex>

namespace mbgl {

namespace gl {
class Context;
using ProcAddress = void (*)();
using FramebufferID = uint32_t;
} // namespace gl

class BackendScope;

class Backend {
public:
    Backend();
    virtual ~Backend();

    // Returns the backend's context which manages OpenGL state.
    gl::Context& getContext();

    // Called prior to rendering to update the internally assumed OpenGL state.
    virtual void updateAssumedState() = 0;

    // Called when the map needs to be rendered; the backend should call Map::render() at some point
    // in the near future. (Not called for Map::renderStill() mode.)
    virtual void invalidate() = 0;

protected:
    // Called with the name of an OpenGL extension that should be loaded. Backend implementations
    // must call the API-specific version that obtains the function pointer for this function,
    // or a null pointer if unsupported/unavailable.
    virtual gl::ProcAddress initializeExtension(const char*) = 0;

    // Called when the backend's GL context needs to be made active or inactive. These are called,
    // as a matched pair, in four situations:
    //
    //   1. When releasing GL resources during Map destruction
    //   2. When calling a CustomLayerInitializeFunction, during Map::addLayer
    //   3. When calling a CustomLayerDeinitializeFunction, during Map::removeLayer
    //   4. When rendering for Map::renderStill
    //
    // They are *not* called for Map::render; it is assumed that the correct context is already
    // activated prior to calling Map::render.
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

} // namespace mbgl
