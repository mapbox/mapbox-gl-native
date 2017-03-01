#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/headless_display.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/map/backend_scope.hpp>

#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace mbgl {

HeadlessBackend::HeadlessBackend() {
}

HeadlessBackend::HeadlessBackend(std::shared_ptr<HeadlessDisplay> display_)
        : display(std::move(display_)) {
}

HeadlessBackend::~HeadlessBackend() {
    BackendScope scope(*this);
    context.reset();
}

void HeadlessBackend::activate() {
    active = true;

    if (!hasContext()) {
        if (!hasDisplay()) {
            throw std::runtime_error("Display is not set");
        }
        createContext();
    }

    assert(hasContext());
    impl->activateContext();

    if (!extensionsLoaded) {
        gl::InitializeExtensions(initializeExtension);
        extensionsLoaded = true;
    }
}

void HeadlessBackend::deactivate() {
    assert(hasContext());
    impl->deactivateContext();
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
