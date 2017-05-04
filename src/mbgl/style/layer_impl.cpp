#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace style {

std::unique_ptr<Layer> Layer::Impl::copy(const std::string& id_,
                                         const std::string& source_) const {
    std::unique_ptr<Layer> result = clone();
    result->baseImpl->id = id_;
    result->baseImpl->source = source_;
    return result;
}

void Layer::Impl::setObserver(LayerObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

} // namespace style
} // namespace mbgl
