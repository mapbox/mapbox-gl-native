#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace style {

void Layer::Impl::setObserver(LayerObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

} // namespace style
} // namespace mbgl
