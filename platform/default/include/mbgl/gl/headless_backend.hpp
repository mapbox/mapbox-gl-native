#pragma once

#include <mbgl/gfx/headless_backend.hpp>
#include <mbgl/gl/renderer_backend.hpp>
#include <memory>
#include <functional>

namespace mbgl {
namespace gl {

class HeadlessBackend final : public gl::RendererBackend, public gfx::HeadlessBackend {
public:
    HeadlessBackend(Size = {256, 256},
                    SwapBehaviour = SwapBehaviour::NoFlush,
                    gfx::ContextMode = gfx::ContextMode::Unique);
    ~HeadlessBackend() override;
    void updateAssumedState() override;
    gfx::Renderable& getDefaultRenderable() override;
    PremultipliedImage readStillImage() override;
    RendererBackend* getRendererBackend() override;

    void swap();

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
    bool active = false;
    SwapBehaviour swapBehaviour = SwapBehaviour::NoFlush;
};

} // namespace gl
} // namespace mbgl
