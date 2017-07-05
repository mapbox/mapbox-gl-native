#pragma once

#include <mbgl/renderer/renderer_backend.hpp>

#include <memory>
#include <functional>

namespace mbgl {

class HeadlessDisplay;

class HeadlessBackend : public RendererBackend {
public:
    HeadlessBackend();
    HeadlessBackend(std::shared_ptr<HeadlessDisplay>);
    ~HeadlessBackend() override;

    void updateAssumedState() override;

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

    bool active = false;
};

} // namespace mbgl
