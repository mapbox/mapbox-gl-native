#include <mbgl/map/backend.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/debugging.hpp>

#include <cassert>

namespace mbgl {

Backend::Backend() = default;

gl::Context& Backend::getContext() {
    assert(BackendScope::exists());
    std::call_once(initialized, [this] {
        context = std::make_unique<gl::Context>();
        gl::debugging::enable();
        gl::initializeExtensions(
            std::bind(&Backend::initializeExtension, this, std::placeholders::_1));
    });
    return *context;
}

Backend::~Backend() = default;

} // namespace mbgl
