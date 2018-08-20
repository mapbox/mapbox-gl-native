#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {

static LayerObserver nullObserver;

Layer::Layer(Immutable<Impl> impl)
    : baseImpl(std::move(impl)),
      observer(&nullObserver) {
}

Layer::~Layer() = default;

LayerType Layer::getType() const {
    return baseImpl->type;
}

std::string Layer::getID() const {
    return baseImpl->id;
}

VisibilityType Layer::getVisibility() const {
    return baseImpl->visibility;
}

float Layer::getMinZoom() const {
    return baseImpl->minZoom;
}

float Layer::getMaxZoom() const {
    return baseImpl->maxZoom;
}

void Layer::setObserver(LayerObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

optional<conversion::Error> Layer::setVisibility(const conversion::Convertible& value) {
    using namespace conversion;

    if (isUndefined(value)) {
        setVisibility(VisibilityType::Visible);
        return nullopt;
    }

    Error error;
    optional<VisibilityType> visibility = convert<VisibilityType>(value, error);
    if (!visibility) {
        return error;
    }

    setVisibility(*visibility);
    return nullopt;
}

} // namespace style
} // namespace mbgl
