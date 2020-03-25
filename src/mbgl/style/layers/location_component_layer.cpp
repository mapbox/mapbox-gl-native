// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/location_component_layer.hpp>
#include <mbgl/style/layers/location_component_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/traits.hpp>

#include <mapbox/eternal.hpp>

namespace mbgl {
namespace style {


// static
const LayerTypeInfo* LocationComponentLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"location-component",
                                        LayerTypeInfo::Source::NotRequired,
                                        LayerTypeInfo::Pass3D::NotRequired,
                                        LayerTypeInfo::Layout::NotRequired,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::NotRequired};
    return &typeInfo;
}


LocationComponentLayer::LocationComponentLayer(const std::string& layerID)
    : Layer(makeMutable<Impl>(layerID, std::string())) {
}

LocationComponentLayer::LocationComponentLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

LocationComponentLayer::~LocationComponentLayer() = default;

const LocationComponentLayer::Impl& LocationComponentLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<LocationComponentLayer::Impl> LocationComponentLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> LocationComponentLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = LocationComponentPaintProperties::Transitionable();
    return std::make_unique<LocationComponentLayer>(std::move(impl_));
}

void LocationComponentLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    layout.stringify(writer);
}

// Layout properties

PropertyValue<float> LocationComponentLayer::getDefaultAccuracyRadius() {
    return AccuracyRadius::defaultValue();
}

const PropertyValue<float>& LocationComponentLayer::getAccuracyRadius() const {
    return impl().layout.get<AccuracyRadius>();
}

void LocationComponentLayer::setAccuracyRadius(const PropertyValue<float>& value) {
    if (value == getAccuracyRadius()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<AccuracyRadius>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationComponentLayer::getDefaultBearing() {
    return Bearing::defaultValue();
}

const PropertyValue<float>& LocationComponentLayer::getBearing() const {
    return impl().layout.get<Bearing>();
}

void LocationComponentLayer::setBearing(const PropertyValue<float>& value) {
    if (value == getBearing()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<Bearing>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<expression::Image> LocationComponentLayer::getDefaultBearingImage() {
    return BearingImage::defaultValue();
}

const PropertyValue<expression::Image>& LocationComponentLayer::getBearingImage() const {
    return impl().layout.get<BearingImage>();
}

void LocationComponentLayer::setBearingImage(const PropertyValue<expression::Image>& value) {
    if (value == getBearingImage()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<BearingImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationComponentLayer::getDefaultBearingImageSize() {
    return BearingImageSize::defaultValue();
}

const PropertyValue<float>& LocationComponentLayer::getBearingImageSize() const {
    return impl().layout.get<BearingImageSize>();
}

void LocationComponentLayer::setBearingImageSize(const PropertyValue<float>& value) {
    if (value == getBearingImageSize()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<BearingImageSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationComponentLayer::getDefaultImageTiltDisplacement() {
    return ImageTiltDisplacement::defaultValue();
}

const PropertyValue<float>& LocationComponentLayer::getImageTiltDisplacement() const {
    return impl().layout.get<ImageTiltDisplacement>();
}

void LocationComponentLayer::setImageTiltDisplacement(const PropertyValue<float>& value) {
    if (value == getImageTiltDisplacement()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<ImageTiltDisplacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<std::array<double, 3>> LocationComponentLayer::getDefaultLocation() {
    return Location::defaultValue();
}

const PropertyValue<std::array<double, 3>>& LocationComponentLayer::getLocation() const {
    return impl().layout.get<Location>();
}

void LocationComponentLayer::setLocation(const PropertyValue<std::array<double, 3>>& value) {
    if (value == getLocation()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<Location>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationComponentLayer::getDefaultPerspectiveCompensation() {
    return PerspectiveCompensation::defaultValue();
}

const PropertyValue<float>& LocationComponentLayer::getPerspectiveCompensation() const {
    return impl().layout.get<PerspectiveCompensation>();
}

void LocationComponentLayer::setPerspectiveCompensation(const PropertyValue<float>& value) {
    if (value == getPerspectiveCompensation()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<PerspectiveCompensation>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<expression::Image> LocationComponentLayer::getDefaultShadowImage() {
    return ShadowImage::defaultValue();
}

const PropertyValue<expression::Image>& LocationComponentLayer::getShadowImage() const {
    return impl().layout.get<ShadowImage>();
}

void LocationComponentLayer::setShadowImage(const PropertyValue<expression::Image>& value) {
    if (value == getShadowImage()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<ShadowImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationComponentLayer::getDefaultShadowImageSize() {
    return ShadowImageSize::defaultValue();
}

const PropertyValue<float>& LocationComponentLayer::getShadowImageSize() const {
    return impl().layout.get<ShadowImageSize>();
}

void LocationComponentLayer::setShadowImageSize(const PropertyValue<float>& value) {
    if (value == getShadowImageSize()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<ShadowImageSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<expression::Image> LocationComponentLayer::getDefaultTopImage() {
    return TopImage::defaultValue();
}

const PropertyValue<expression::Image>& LocationComponentLayer::getTopImage() const {
    return impl().layout.get<TopImage>();
}

void LocationComponentLayer::setTopImage(const PropertyValue<expression::Image>& value) {
    if (value == getTopImage()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TopImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationComponentLayer::getDefaultTopImageSize() {
    return TopImageSize::defaultValue();
}

const PropertyValue<float>& LocationComponentLayer::getTopImageSize() const {
    return impl().layout.get<TopImageSize>();
}

void LocationComponentLayer::setTopImageSize(const PropertyValue<float>& value) {
    if (value == getTopImageSize()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TopImageSize>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Paint properties

PropertyValue<Color> LocationComponentLayer::getDefaultAccuracyRadiusBorderColor() {
    return {Color::white()};
}

const PropertyValue<Color>& LocationComponentLayer::getAccuracyRadiusBorderColor() const {
    return impl().paint.template get<AccuracyRadiusBorderColor>().value;
}

void LocationComponentLayer::setAccuracyRadiusBorderColor(const PropertyValue<Color>& value) {
    if (value == getAccuracyRadiusBorderColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusBorderColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationComponentLayer::setAccuracyRadiusBorderColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusBorderColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationComponentLayer::getAccuracyRadiusBorderColorTransition() const {
    return impl().paint.template get<AccuracyRadiusBorderColor>().options;
}

PropertyValue<Color> LocationComponentLayer::getDefaultAccuracyRadiusColor() {
    return {Color::white()};
}

const PropertyValue<Color>& LocationComponentLayer::getAccuracyRadiusColor() const {
    return impl().paint.template get<AccuracyRadiusColor>().value;
}

void LocationComponentLayer::setAccuracyRadiusColor(const PropertyValue<Color>& value) {
    if (value == getAccuracyRadiusColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationComponentLayer::setAccuracyRadiusColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationComponentLayer::getAccuracyRadiusColorTransition() const {
    return impl().paint.template get<AccuracyRadiusColor>().options;
}

using namespace conversion;

namespace {

constexpr uint8_t kPaintPropertyCount = 4u;

enum class Property : uint8_t {
    AccuracyRadiusBorderColor,
    AccuracyRadiusColor,
    AccuracyRadiusBorderColorTransition,
    AccuracyRadiusColorTransition,
    AccuracyRadius = kPaintPropertyCount,
    Bearing,
    BearingImage,
    BearingImageSize,
    ImageTiltDisplacement,
    Location,
    PerspectiveCompensation,
    ShadowImage,
    ShadowImageSize,
    TopImage,
    TopImageSize,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"accuracy-radius-border-color", toUint8(Property::AccuracyRadiusBorderColor)},
     {"accuracy-radius-color", toUint8(Property::AccuracyRadiusColor)},
     {"accuracy-radius-border-color-transition", toUint8(Property::AccuracyRadiusBorderColorTransition)},
     {"accuracy-radius-color-transition", toUint8(Property::AccuracyRadiusColorTransition)},
     {"accuracy-radius", toUint8(Property::AccuracyRadius)},
     {"bearing", toUint8(Property::Bearing)},
     {"bearing-image", toUint8(Property::BearingImage)},
     {"bearing-image-size", toUint8(Property::BearingImageSize)},
     {"image-tilt-displacement", toUint8(Property::ImageTiltDisplacement)},
     {"location", toUint8(Property::Location)},
     {"perspective-compensation", toUint8(Property::PerspectiveCompensation)},
     {"shadow-image", toUint8(Property::ShadowImage)},
     {"shadow-image-size", toUint8(Property::ShadowImageSize)},
     {"top-image", toUint8(Property::TopImage)},
     {"top-image-size", toUint8(Property::TopImageSize)}});

StyleProperty getLayerProperty(const LocationComponentLayer& layer, Property property) {
    switch (property) {
        case Property::AccuracyRadiusBorderColor:
            return makeStyleProperty(layer.getAccuracyRadiusBorderColor());
        case Property::AccuracyRadiusColor:
            return makeStyleProperty(layer.getAccuracyRadiusColor());
        case Property::AccuracyRadiusBorderColorTransition:
            return makeStyleProperty(layer.getAccuracyRadiusBorderColorTransition());
        case Property::AccuracyRadiusColorTransition:
            return makeStyleProperty(layer.getAccuracyRadiusColorTransition());
        case Property::AccuracyRadius:
            return makeStyleProperty(layer.getAccuracyRadius());
        case Property::Bearing:
            return makeStyleProperty(layer.getBearing());
        case Property::BearingImage:
            return makeStyleProperty(layer.getBearingImage());
        case Property::BearingImageSize:
            return makeStyleProperty(layer.getBearingImageSize());
        case Property::ImageTiltDisplacement:
            return makeStyleProperty(layer.getImageTiltDisplacement());
        case Property::Location:
            return makeStyleProperty(layer.getLocation());
        case Property::PerspectiveCompensation:
            return makeStyleProperty(layer.getPerspectiveCompensation());
        case Property::ShadowImage:
            return makeStyleProperty(layer.getShadowImage());
        case Property::ShadowImageSize:
            return makeStyleProperty(layer.getShadowImageSize());
        case Property::TopImage:
            return makeStyleProperty(layer.getTopImage());
        case Property::TopImageSize:
            return makeStyleProperty(layer.getTopImageSize());
    }
    return {};
}

StyleProperty getLayerProperty(const LocationComponentLayer& layer, const std::string& name) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }
    return getLayerProperty(layer, static_cast<Property>(it->second));
}

} // namespace

Value LocationComponentLayer::serialize() const {
    auto result = Layer::serialize();
    assert(result.getObject());
    for (const auto& property : layerProperties) {
        auto styleProperty = getLayerProperty(*this, static_cast<Property>(property.second));
        if (styleProperty.getKind() == StyleProperty::Kind::Undefined) continue;
        serializeProperty(result, styleProperty, property.first.c_str(), property.second < kPaintPropertyCount);
    }
    return result;
}

optional<Error> LocationComponentLayer::setPropertyInternal(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) return Error{"layer doesn't support this property"};

    auto property = static_cast<Property>(it->second);

    if (property == Property::AccuracyRadiusBorderColor || property == Property::AccuracyRadiusColor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::AccuracyRadiusBorderColor) {
            setAccuracyRadiusBorderColor(*typedValue);
            return nullopt;
        }

        if (property == Property::AccuracyRadiusColor) {
            setAccuracyRadiusColor(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::AccuracyRadius || property == Property::Bearing ||
        property == Property::BearingImageSize || property == Property::ImageTiltDisplacement ||
        property == Property::PerspectiveCompensation || property == Property::ShadowImageSize ||
        property == Property::TopImageSize) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::AccuracyRadius) {
            setAccuracyRadius(*typedValue);
            return nullopt;
        }

        if (property == Property::Bearing) {
            setBearing(*typedValue);
            return nullopt;
        }

        if (property == Property::BearingImageSize) {
            setBearingImageSize(*typedValue);
            return nullopt;
        }

        if (property == Property::ImageTiltDisplacement) {
            setImageTiltDisplacement(*typedValue);
            return nullopt;
        }

        if (property == Property::PerspectiveCompensation) {
            setPerspectiveCompensation(*typedValue);
            return nullopt;
        }

        if (property == Property::ShadowImageSize) {
            setShadowImageSize(*typedValue);
            return nullopt;
        }

        if (property == Property::TopImageSize) {
            setTopImageSize(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::BearingImage || property == Property::ShadowImage || property == Property::TopImage) {
        Error error;
        const auto& typedValue = convert<PropertyValue<expression::Image>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::BearingImage) {
            setBearingImage(*typedValue);
            return nullopt;
        }

        if (property == Property::ShadowImage) {
            setShadowImage(*typedValue);
            return nullopt;
        }

        if (property == Property::TopImage) {
            setTopImage(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::Location) {
        Error error;
        const auto& typedValue = convert<PropertyValue<std::array<double, 3>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setLocation(*typedValue);
        return nullopt;
    }

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }

    if (property == Property::AccuracyRadiusBorderColorTransition) {
        setAccuracyRadiusBorderColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::AccuracyRadiusColorTransition) {
        setAccuracyRadiusColorTransition(*transition);
        return nullopt;
    }

    return Error{"layer doesn't support this property"};
}

StyleProperty LocationComponentLayer::getProperty(const std::string& name) const {
    return getLayerProperty(*this, name);
}

Mutable<Layer::Impl> LocationComponentLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl

// clang-format on
