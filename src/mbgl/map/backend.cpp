#include <mbgl/map/backend.hpp>
#include <mbgl/gl/context.hpp>

#include <cassert>

namespace mbgl {

Backend::Backend() : context(std::make_unique<gl::Context>()) {
}

gl::Context& Backend::getContext() {
    return *context;
}

Backend::~Backend() = default;

} // namespace mbgl
