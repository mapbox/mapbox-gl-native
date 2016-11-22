#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/headless_display.hpp>

#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace mbgl {

HeadlessBackend::HeadlessBackend() {
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

    if (!hasContext()) {
        if (!hasDisplay()) {
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

void HeadlessBackend::destroyContext() {
    assert(hasContext());
    impl.reset();
}

void HeadlessBackend::activateContext() {
    assert(hasContext());
    impl->activateContext();
}

void HeadlessBackend::deactivateContext() {
    assert(hasContext());
    impl->deactivateContext();
}

void HeadlessBackend::notifyMapChange(MapChange change) {
    if (mapChangeCallback) {
        mapChangeCallback(change);
    }
}

} // namespace mbgl
