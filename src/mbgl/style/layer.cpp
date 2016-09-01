#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace style {

Layer::Layer(Type type_, std::unique_ptr<Impl> baseImpl_)
    : type(type_), baseImpl(std::move(baseImpl_)) {
}

Layer::~Layer() = default;

const std::string& Layer::getID() const {
    return baseImpl->id;
}

VisibilityType Layer::getVisibility() const {
    return baseImpl->visibility;
}

void Layer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    baseImpl->visibility = value;
    baseImpl->observer->onLayerVisibilityChanged(*this);
}

float Layer::getMinZoom() const {
    return baseImpl->minZoom;
}

void Layer::setMinZoom(float minZoom) const {
    baseImpl->minZoom = minZoom;
}

float Layer::getMaxZoom() const {
    return baseImpl->maxZoom;
}

void Layer::setMaxZoom(float maxZoom) const {
    baseImpl->maxZoom = maxZoom;
}

} // namespace style
} // namespace mbgl
