#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>

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

optional<std::string> LayerFactory::getSource(const conversion::Convertible& value) const {
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

bool LayerFactory::initSourceLayerAndFilter(Layer* layer, const conversion::Convertible& value) const {
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
        conversion::Error error;
        optional<Filter> filter = conversion::convert<Filter>(*filterValue, error);
        if (!filter) {
            return false;
        }
        layer->setFilter(*filter);
    }

    return true;
}

// TODO: Move the LayerManager implementation to the dedicated .cpp file per platform.
class LayerManagerImpl : public LayerManager {
public:
    void addLayerFactory(LayerFactory* factory) {
        factories.emplace(std::make_pair(factory->type(), std::unique_ptr<LayerFactory>(factory)));
    }
private:
    // LayerManager overrides.
    std::unique_ptr<Layer> createLayer(const std::string& type, const std::string& id, const conversion::Convertible& value, conversion::Error& error) final;

    std::unordered_map<std::string, std::unique_ptr<LayerFactory>> factories;
};

std::unique_ptr<Layer> LayerManagerImpl::createLayer(const std::string& type, const std::string& id, const conversion::Convertible& value, conversion::Error& error) {
    auto search = factories.find(type);
    if (search != factories.end()) {
        if (auto layer = search->second->createLayer(id, value)) {
            return layer;
        }
        error.message = "Error parsing a layer of type: " + type;
    } else {
        error.message = "Unsupported layer type: " + type;
    }
    return nullptr;
}

// static 
LayerManager* LayerManager::get() {
    static LayerManager* instance = nullptr;
    if (instance == nullptr) {
       static LayerManagerImpl impl;
       impl.addLayerFactory(new FillLayerFactory);
       impl.addLayerFactory(new LineLayerFactory);
       impl.addLayerFactory(new CircleLayerFactory);
       impl.addLayerFactory(new SymbolLayerFactory);
       impl.addLayerFactory(new RasterLayerFactory);
       impl.addLayerFactory(new BackgroundLayerFactory);
       impl.addLayerFactory(new HillshadeLayerFactory);
       impl.addLayerFactory(new FillExtrusionLayerFactory);
       impl.addLayerFactory(new HeatmapLayerFactory);

       instance = &impl;
    }
    return instance;
}


} // namespace style
} // namespace mbgl
