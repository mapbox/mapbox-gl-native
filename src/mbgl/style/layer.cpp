#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/tile/tile.hpp>

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace style {

static_assert(mbgl::underlying_type(Tile::Kind::Geometry) == mbgl::underlying_type(LayerTypeInfo::TileKind::Geometry),
              "tile kind error");
static_assert(mbgl::underlying_type(Tile::Kind::Raster) == mbgl::underlying_type(LayerTypeInfo::TileKind::Raster),
              "tile kind error");
static_assert(mbgl::underlying_type(Tile::Kind::RasterDEM) == mbgl::underlying_type(LayerTypeInfo::TileKind::RasterDEM),
              "tile kind error");

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

Value Layer::serialize() const {
    mapbox::base::ValueObject result;
    result.emplace(std::make_pair("id", getID()));
    result.emplace(std::make_pair("type", Layer::getTypeInfo()->type));

    auto source = getSourceID();
    if (!source.empty()) {
        result.emplace(std::make_pair("source", std::move(source)));
    }

    auto sourceLayer = getSourceLayer();
    if (!sourceLayer.empty()) {
        result.emplace(std::make_pair("source-layer", std::move(sourceLayer)));
    }

    if (getFilter()) {
        result.emplace(std::make_pair("filter", getFilter().serialize()));
    }

    if (getMinZoom() != -std::numeric_limits<float>::infinity()) {
        result.emplace(std::make_pair("minzoom", getMinZoom()));
    }

    if (getMaxZoom() != std::numeric_limits<float>::infinity()) {
        result.emplace(std::make_pair("maxzoom", getMaxZoom()));
    }

    if (getVisibility() == VisibilityType::None) {
        result["layout"] = mapbox::base::ValueObject{std::make_pair("visibility", "none")};
    }

    return result;
}

void Layer::serializeProperty(Value& out, const StyleProperty& property, const char* propertyName, bool isPaint) const {
    assert(out.getObject());
    auto& object = *(out.getObject());
    std::string propertyType = isPaint ? "paint" : "layout";
    auto it = object.find(propertyType);
    auto pair = std::make_pair(std::string(propertyName), Value{property.getValue()});
    if (it != object.end()) {
        assert(it->second.getObject());
        it->second.getObject()->emplace(std::move(pair));
    } else {
        object[propertyType] = mapbox::base::ValueObject{{std::move(pair)}};
    }
}

void Layer::setObserver(LayerObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

optional<conversion::Error> Layer::setProperty(const std::string& name, const conversion::Convertible& value) {
    using namespace conversion;
    optional<Error> error = setPropertyInternal(name, value);
    if (!error) return error; // Successfully set by the derived class implementation.
    if (name == "visibility") return setVisibility(value);
    if (name == "minzoom") {
        if (auto zoom = convert<float>(value, *error)) {
            setMinZoom(*zoom);
            return nullopt;
        }
    } else if (name == "maxzoom") {
        if (auto zoom = convert<float>(value, *error)) {
            setMaxZoom(*zoom);
            return nullopt;
        }
    } else if (name == "filter") {
        if (auto filter = convert<Filter>(value, *error)) {
            setFilter(*filter);
            return nullopt;
        }
    } else if (name == "source-layer") {
        if (auto sourceLayer = convert<std::string>(value, *error)) {
            if (getTypeInfo()->source != LayerTypeInfo::Source::Required) {
                Log::Warning(mbgl::Event::General,
                             "source-layer property cannot be applied to"
                             "the layer %s",
                             baseImpl->id.c_str());
                return nullopt;
            }
            setSourceLayer(*sourceLayer);
            return nullopt;
        }
    }
    return error;
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
} // namespace mbgl
