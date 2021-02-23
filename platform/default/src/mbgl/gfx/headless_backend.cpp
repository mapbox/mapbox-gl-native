#include <mbgl/gfx/headless_backend.hpp>

namespace mbgl {
namespace gfx {

HeadlessBackend::HeadlessBackend(Size size_)
    : mbgl::gfx::Renderable(size_, nullptr) {
}

void HeadlessBackend::setSize(Size size_) {
    size = size_;
    resource.reset();
}

} // namespace gfx
} // namespace mbgl
