#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <mbgl/renderer/render_layer.hpp>

namespace mbgl {
namespace style {

static LayerObserver nullObserver;

Layer::Layer(Immutable<Impl> impl)
    : baseImpl(std::move(impl)),
      observer(&nullObserver) {
}

Layer::~Layer() = default;

std::string Layer::getID() const {
    return baseImpl->id;
}

std::string Layer::getSourceID() const {
    return baseImpl->source;
}

std::string Layer::getSourceLayer() const {
    return baseImpl->sourceLayer;
}

void Layer::setSourceLayer(const std::string& sourceLayer) {
    auto impl_ = mutableBaseImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

const Filter& Layer::getFilter() const {
    return baseImpl->filter;
}

void Layer::setFilter(const Filter& filter) {
    auto impl_ = mutableBaseImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

VisibilityType Layer::getVisibility() const {
    return baseImpl->visibility;
}

void Layer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableBaseImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

float Layer::getMinZoom() const {
    return baseImpl->minZoom;
}

float Layer::getMaxZoom() const {
    return baseImpl->maxZoom;
}

void Layer::setMinZoom(float minZoom) {
    auto impl_ = mutableBaseImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void Layer::setMaxZoom(float maxZoom) {
    auto impl_ = mutableBaseImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
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

const LayerTypeInfo* Layer::getTypeInfo() const noexcept {
    return baseImpl->getTypeInfo();
}

} // namespace style

optional<std::string> LayerFactory::getSource(const style::conversion::Convertible& value) const noexcept {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        return nullopt;
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        return nullopt;
    }

    return source;
}

bool LayerFactory::initSourceLayerAndFilter(style::Layer* layer, const style::conversion::Convertible& value) const noexcept {
    auto sourceLayerValue = objectMember(value, "source-layer");
    if (sourceLayerValue) {
        optional<std::string> sourceLayer = toString(*sourceLayerValue);
        if (!sourceLayer) {
            return false;
        }
        layer->setSourceLayer(*sourceLayer);
    }

    auto filterValue = objectMember(value, "filter");
    if (filterValue) {
        style::conversion::Error error;
        optional<style::Filter> filter = style::conversion::convert<style::Filter>(*filterValue, error);
        if (!filter) {
            return false;
        }
        layer->setFilter(*filter);
    }

    return true;
}

std::unique_ptr<style::Layer> LayerManager::createLayer(
    const std::string& type, const std::string& id,
    const style::conversion::Convertible& value, style::conversion::Error& error) noexcept {
    if (LayerFactory* factory = getFactory(type)) {
        auto layer = factory->createLayer(id, value);
        if (!layer) {
            error.message = "Error parsing a layer of type: " + type;
        }
        return layer;
    }
    error.message = "Unsupported layer type: " + type;
    return nullptr;
}

std::unique_ptr<RenderLayer> LayerManager::createRenderLayer(Immutable<style::Layer::Impl> impl) noexcept {
    LayerFactory* factory = getFactory(impl->getTypeInfo());
    assert(factory);
    return factory->createRenderLayer(std::move(impl));
}


} // namespace mbgl
