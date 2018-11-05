#include <mbgl/style/layers/custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

CustomLayer::CustomLayer(const std::string& layerID,
                         std::unique_ptr<CustomLayerHost> host)
    : Layer(makeMutable<Impl>(layerID, std::move(host))) {
}

CustomLayer::~CustomLayer() = default;

const CustomLayer::Impl& CustomLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<CustomLayer::Impl> CustomLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> CustomLayer::cloneRef(const std::string&) const {
    assert(false);
    return nullptr;
}

using namespace conversion;

optional<Error> CustomLayer::setPaintProperty(const std::string&, const Convertible&) {
    return Error { "layer doesn't support this property" };
}

optional<Error> CustomLayer::setLayoutProperty(const std::string&, const Convertible&) {
    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> CustomLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

CustomLayerFactory* CustomLayerFactory::instance = nullptr;

CustomLayerFactory::CustomLayerFactory() {
    assert(!instance);
    instance = this;
}

CustomLayerFactory::~CustomLayerFactory() = default;

// static
CustomLayerFactory* CustomLayerFactory::get() {
    assert(instance);
    return instance;
}

bool CustomLayerFactory::supportsType(const std::string&) const {
    return false;
}

std::unique_ptr<style::Layer> CustomLayerFactory::createLayer(const std::string&, const conversion::Convertible&) {
    assert(false);
    return nullptr;
}

} // namespace style
} // namespace mbgl
