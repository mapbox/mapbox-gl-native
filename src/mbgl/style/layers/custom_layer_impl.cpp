#include <mbgl/style/layers/custom_layer_impl.hpp>

namespace mbgl {
namespace style {

CustomLayer::Impl::Impl(const std::string& id_,
                        std::unique_ptr<CustomLayerHost> host_)
    : Layer::Impl(LayerType::Custom, id_, std::string()) {
    host = std::move(host_);
}

bool CustomLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void CustomLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

} // namespace style
} // namespace mbgl
