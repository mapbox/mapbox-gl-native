#pragma once

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>

#include <memory>
#include <functional>

namespace mbgl {
namespace gl {

class HeadlessBackend final : public gl::RendererBackend, public gfx::Renderable {
public:
    HeadlessBackend(Size = { 256, 256 }, gfx::ContextMode = gfx::ContextMode::Unique);
    ~HeadlessBackend() override;

    gfx::Renderable& getDefaultRenderable() override;

    Size getFramebufferSize() const;
    void updateAssumedState() override;

    void setSize(Size);
    PremultipliedImage readStillImage();

    class Impl {
    public:
        virtual ~Impl() = default;
        virtual gl::ProcAddress getExtensionFunctionPointer(const char*) = 0;
        virtual void activateContext() = 0;
        virtual void deactivateContext() {}
    };

private:
    // Implementation specific functions
    gl::ProcAddress getExtensionFunctionPointer(const char*) override;

    void activate() override;
    void deactivate() override;

    void createImpl();

private:
    std::unique_ptr<Impl> impl;

    float pixelRatio;
    bool active = false;
};

} // namespace gl
} // namespace mbgl
