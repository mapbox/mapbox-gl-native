#pragma once

#include <mbgl/gl/extension.hpp>

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

    void invalidate() override;
    void activate() override;
    void deactivate() override;
    void notifyMapChange(MapChange) override;

    void setMapChangeCallback(std::function<void(MapChange)>&& cb) { mapChangeCallback = std::move(cb); }

    struct Impl {
        virtual ~Impl() {}
        virtual void activateContext() = 0;
        virtual void deactivateContext() {}
    };

private:
    // Implementation specific functions
    static gl::glProc initializeExtension(const char*);

    bool hasContext() const { return bool(impl); }
    bool hasDisplay();

    void createContext();

private:
    void destroyContext();

    void activateContext();
    void deactivateContext();

    std::unique_ptr<Impl> impl;
    std::shared_ptr<HeadlessDisplay> display;

    bool extensionsLoaded = false;
    bool active = false;

    std::function<void(MapChange)> mapChangeCallback;
};

} // namespace mbgl
