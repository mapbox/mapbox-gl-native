#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/headless_display.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/renderer/backend_scope.hpp>

#include <cassert>
#include <stdexcept>
#include <type_traits>

namespace mbgl {

HeadlessBackend::HeadlessBackend() = default;

HeadlessBackend::HeadlessBackend(std::shared_ptr<HeadlessDisplay> display_)
        : display(std::move(display_)) {
}

HeadlessBackend::~HeadlessBackend() {
    BackendScope guard { *this, getScopeType() };
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
}

void HeadlessBackend::deactivate() {
    assert(hasContext());
    impl->deactivateContext();
    active = false;
}

void HeadlessBackend::updateAssumedState() {
    // no-op
}

} // namespace mbgl
