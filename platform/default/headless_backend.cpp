#include <mbgl/platform/default/headless_backend.hpp>
#include <mbgl/platform/default/headless_display.hpp>

#include <cassert>
#include <stdexcept>

namespace mbgl {

HeadlessBackend::HeadlessBackend() : display(std::make_shared<HeadlessDisplay>()) {
    activate();
}

HeadlessBackend::HeadlessBackend(std::shared_ptr<HeadlessDisplay> display_)
    : display(std::move(display_)) {
    activate();
}

HeadlessBackend::~HeadlessBackend() {
    deactivate();
    destroyContext();
}

void HeadlessBackend::activate() {
    active = true;

    if (!glContext) {
        if (!display) {
            throw std::runtime_error("Display is not set");
        }
        createContext();
    }

    activateContext();

    if (!extensionsLoaded) {
        gl::InitializeExtensions(initializeExtension);
        extensionsLoaded = true;
    }
}

void HeadlessBackend::deactivate() {
    deactivateContext();
    active = false;
}

void HeadlessBackend::invalidate() {
    assert(false);
}

void HeadlessBackend::notifyMapChange(MapChange change) {
    if (mapChangeCallback) {
        mapChangeCallback(change);
    }
}

} // namespace mbgl
