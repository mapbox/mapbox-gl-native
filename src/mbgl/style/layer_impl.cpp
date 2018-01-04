#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace style {

Layer::Impl::Impl(LayerType type_, std::string layerID, std::string sourceID)
    : type(type_),
      id(std::move(layerID)),
      source(std::move(sourceID)) {
}

} // namespace style
} // namespace mbgl
