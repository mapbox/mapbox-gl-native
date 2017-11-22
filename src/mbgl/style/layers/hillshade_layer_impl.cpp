#include <mbgl/style/layers/hillshade_layer_impl.hpp>

namespace mbgl {
namespace style {

bool HillshadeLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

} // namespace style
} // namespace mbgl
