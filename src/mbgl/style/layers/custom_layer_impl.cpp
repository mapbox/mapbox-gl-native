#include <mbgl/style/layers/custom_layer_impl.hpp>

namespace mbgl {
namespace style {

CustomLayer::Impl::~Impl() {
}

CustomLayer::Impl::Impl(const std::string& id_,
                        std::unique_ptr<CustomLayerContext> context_):
    Layer::Impl(LayerType::Custom, id_, std::string()),
    context(std::move(context_)) {

}

void CustomLayer::Impl::didSetObserver(bool didSet) const {
    if (didSet) {
        context->attach();
    }
    else {
        context->detach();
    }
}

bool CustomLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void CustomLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

} // namespace style
} // namespace mbgl
