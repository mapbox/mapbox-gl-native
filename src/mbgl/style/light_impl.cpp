#include <mbgl/style/light_impl.hpp>

namespace mbgl {
namespace style {

void Light::Impl::setObserver(LightObserver* observer_) {
    observer = observer_;
}

} // namespace style
} // namespace mbgl
