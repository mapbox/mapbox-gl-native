#pragma once

#include <mbgl/map/backend.hpp>

#include <memory>
#include <functional>

namespace mbgl {

class HeadlessDisplay;

class HeadlessBackend : public Backend {
public:
    HeadlessBackend();
    HeadlessBackend(std::shared_ptr<HeadlessDisplay>);
    ~HeadlessBackend() override;

    void updateAssumedState() override;

    void invalidate() override;

    struct Impl {
        virtual ~Impl() {}
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

    std::unique_ptr<Impl> impl;
    std::shared_ptr<HeadlessDisplay> display;

    bool active = false;
};

} // namespace mbgl
