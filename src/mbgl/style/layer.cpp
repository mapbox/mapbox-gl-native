#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/tile/tile.hpp>

#include <mbgl/renderer/render_layer.hpp>

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
} // namespace mbgl
