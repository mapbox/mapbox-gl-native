#pragma once

#include <mbgl/renderer/renderer_backend.hpp>

#include <memory>
#include <functional>

namespace mbgl {

class HeadlessDisplay;

class HeadlessBackend : public RendererBackend {
public:
    HeadlessBackend(Size = { 256, 256 });
    ~HeadlessBackend() override;

    void bind() override;
    Size getFramebufferSize() const override;
    void updateAssumedState() override;

    void setSize(Size);
    PremultipliedImage readStillImage();

    struct Impl {
        virtual ~Impl() = default;
        virtual void activateContext() = 0;
        virtual void deactivateContext() {}
    };

private:
    // Implementation specific functions
    gl::ProcAddress initializeExtension(const char*) override;

    void activate() override;
    void deactivate() override;

    bool hasContext() const { return bool(impl); }
    bool hasDisplay();

    void createContext();

    std::shared_ptr<HeadlessDisplay> display;
    std::unique_ptr<Impl> impl;

    Size size;
    float pixelRatio;
    bool active = false;

    class View;
    std::unique_ptr<View> view;
};

} // namespace mbgl
