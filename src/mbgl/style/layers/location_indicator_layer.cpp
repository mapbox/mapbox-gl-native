// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/location_indicator_layer.hpp>
#include <mbgl/style/layers/location_indicator_layer_impl.hpp>
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
const LayerTypeInfo* LocationIndicatorLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"location-indicator",
                                        LayerTypeInfo::Source::NotRequired,
                                        LayerTypeInfo::Pass3D::NotRequired,
                                        LayerTypeInfo::Layout::NotRequired,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::NotRequired};
    return &typeInfo;
}


LocationIndicatorLayer::LocationIndicatorLayer(const std::string& layerID)
    : Layer(makeMutable<Impl>(layerID, std::string())) {
}

LocationIndicatorLayer::LocationIndicatorLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

LocationIndicatorLayer::~LocationIndicatorLayer() = default;

const LocationIndicatorLayer::Impl& LocationIndicatorLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<LocationIndicatorLayer::Impl> LocationIndicatorLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> LocationIndicatorLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = LocationIndicatorPaintProperties::Transitionable();
    return std::make_unique<LocationIndicatorLayer>(std::move(impl_));
}

void LocationIndicatorLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    layout.stringify(writer);
}

// Layout properties

PropertyValue<expression::Image> LocationIndicatorLayer::getDefaultBearingImage() {
    return BearingImage::defaultValue();
}

const PropertyValue<expression::Image>& LocationIndicatorLayer::getBearingImage() const {
    return impl().layout.get<BearingImage>();
}

void LocationIndicatorLayer::setBearingImage(const PropertyValue<expression::Image>& value) {
    if (value == getBearingImage()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<BearingImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationIndicatorLayer::getDefaultImageTiltDisplacement() {
    return ImageTiltDisplacement::defaultValue();
}

const PropertyValue<float>& LocationIndicatorLayer::getImageTiltDisplacement() const {
    return impl().layout.get<ImageTiltDisplacement>();
}

void LocationIndicatorLayer::setImageTiltDisplacement(const PropertyValue<float>& value) {
    if (value == getImageTiltDisplacement()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<ImageTiltDisplacement>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LocationIndicatorLayer::getDefaultPerspectiveCompensation() {
    return PerspectiveCompensation::defaultValue();
}

const PropertyValue<float>& LocationIndicatorLayer::getPerspectiveCompensation() const {
    return impl().layout.get<PerspectiveCompensation>();
}

void LocationIndicatorLayer::setPerspectiveCompensation(const PropertyValue<float>& value) {
    if (value == getPerspectiveCompensation()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<PerspectiveCompensation>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<expression::Image> LocationIndicatorLayer::getDefaultShadowImage() {
    return ShadowImage::defaultValue();
}

const PropertyValue<expression::Image>& LocationIndicatorLayer::getShadowImage() const {
    return impl().layout.get<ShadowImage>();
}

void LocationIndicatorLayer::setShadowImage(const PropertyValue<expression::Image>& value) {
    if (value == getShadowImage()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<ShadowImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<expression::Image> LocationIndicatorLayer::getDefaultTopImage() {
    return TopImage::defaultValue();
}

const PropertyValue<expression::Image>& LocationIndicatorLayer::getTopImage() const {
    return impl().layout.get<TopImage>();
}

void LocationIndicatorLayer::setTopImage(const PropertyValue<expression::Image>& value) {
    if (value == getTopImage()) return;
    auto impl_ = mutableImpl();
    impl_->layout.get<TopImage>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Paint properties

PropertyValue<float> LocationIndicatorLayer::getDefaultAccuracyRadius() {
    return {0};
}

const PropertyValue<float>& LocationIndicatorLayer::getAccuracyRadius() const {
    return impl().paint.template get<AccuracyRadius>().value;
}

void LocationIndicatorLayer::setAccuracyRadius(const PropertyValue<float>& value) {
    if (value == getAccuracyRadius())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadius>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setAccuracyRadiusTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadius>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getAccuracyRadiusTransition() const {
    return impl().paint.template get<AccuracyRadius>().options;
}

PropertyValue<Color> LocationIndicatorLayer::getDefaultAccuracyRadiusBorderColor() {
    return {Color::white()};
}

const PropertyValue<Color>& LocationIndicatorLayer::getAccuracyRadiusBorderColor() const {
    return impl().paint.template get<AccuracyRadiusBorderColor>().value;
}

void LocationIndicatorLayer::setAccuracyRadiusBorderColor(const PropertyValue<Color>& value) {
    if (value == getAccuracyRadiusBorderColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusBorderColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setAccuracyRadiusBorderColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusBorderColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getAccuracyRadiusBorderColorTransition() const {
    return impl().paint.template get<AccuracyRadiusBorderColor>().options;
}

PropertyValue<Color> LocationIndicatorLayer::getDefaultAccuracyRadiusColor() {
    return {Color::white()};
}

const PropertyValue<Color>& LocationIndicatorLayer::getAccuracyRadiusColor() const {
    return impl().paint.template get<AccuracyRadiusColor>().value;
}

void LocationIndicatorLayer::setAccuracyRadiusColor(const PropertyValue<Color>& value) {
    if (value == getAccuracyRadiusColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setAccuracyRadiusColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<AccuracyRadiusColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getAccuracyRadiusColorTransition() const {
    return impl().paint.template get<AccuracyRadiusColor>().options;
}

PropertyValue<Rotation> LocationIndicatorLayer::getDefaultBearing() {
    return {0};
}

const PropertyValue<Rotation>& LocationIndicatorLayer::getBearing() const {
    return impl().paint.template get<Bearing>().value;
}

void LocationIndicatorLayer::setBearing(const PropertyValue<Rotation>& value) {
    if (value == getBearing())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<Bearing>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setBearingTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<Bearing>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getBearingTransition() const {
    return impl().paint.template get<Bearing>().options;
}

PropertyValue<float> LocationIndicatorLayer::getDefaultBearingImageSize() {
    return {0};
}

const PropertyValue<float>& LocationIndicatorLayer::getBearingImageSize() const {
    return impl().paint.template get<BearingImageSize>().value;
}

void LocationIndicatorLayer::setBearingImageSize(const PropertyValue<float>& value) {
    if (value == getBearingImageSize())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BearingImageSize>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setBearingImageSizeTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BearingImageSize>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getBearingImageSizeTransition() const {
    return impl().paint.template get<BearingImageSize>().options;
}

PropertyValue<std::array<double, 3>> LocationIndicatorLayer::getDefaultLocation() {
    return {{{0, 0, 0}}};
}

const PropertyValue<std::array<double, 3>>& LocationIndicatorLayer::getLocation() const {
    return impl().paint.template get<Location>().value;
}

void LocationIndicatorLayer::setLocation(const PropertyValue<std::array<double, 3>>& value) {
    if (value == getLocation())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<Location>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setLocationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<Location>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getLocationTransition() const {
    return impl().paint.template get<Location>().options;
}

PropertyValue<float> LocationIndicatorLayer::getDefaultShadowImageSize() {
    return {0};
}

const PropertyValue<float>& LocationIndicatorLayer::getShadowImageSize() const {
    return impl().paint.template get<ShadowImageSize>().value;
}

void LocationIndicatorLayer::setShadowImageSize(const PropertyValue<float>& value) {
    if (value == getShadowImageSize())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<ShadowImageSize>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setShadowImageSizeTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<ShadowImageSize>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getShadowImageSizeTransition() const {
    return impl().paint.template get<ShadowImageSize>().options;
}

PropertyValue<float> LocationIndicatorLayer::getDefaultTopImageSize() {
    return {0};
}

const PropertyValue<float>& LocationIndicatorLayer::getTopImageSize() const {
    return impl().paint.template get<TopImageSize>().value;
}

void LocationIndicatorLayer::setTopImageSize(const PropertyValue<float>& value) {
    if (value == getTopImageSize())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<TopImageSize>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LocationIndicatorLayer::setTopImageSizeTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<TopImageSize>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LocationIndicatorLayer::getTopImageSizeTransition() const {
    return impl().paint.template get<TopImageSize>().options;
}

using namespace conversion;

namespace {

constexpr uint8_t kPaintPropertyCount = 16u;

enum class Property : uint8_t {
    AccuracyRadius,
    AccuracyRadiusBorderColor,
    AccuracyRadiusColor,
    Bearing,
    BearingImageSize,
    Location,
    ShadowImageSize,
    TopImageSize,
    AccuracyRadiusTransition,
    AccuracyRadiusBorderColorTransition,
    AccuracyRadiusColorTransition,
    BearingTransition,
    BearingImageSizeTransition,
    LocationTransition,
    ShadowImageSizeTransition,
    TopImageSizeTransition,
    BearingImage = kPaintPropertyCount,
    ImageTiltDisplacement,
    PerspectiveCompensation,
    ShadowImage,
    TopImage,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"accuracy-radius", toUint8(Property::AccuracyRadius)},
     {"accuracy-radius-border-color", toUint8(Property::AccuracyRadiusBorderColor)},
     {"accuracy-radius-color", toUint8(Property::AccuracyRadiusColor)},
     {"bearing", toUint8(Property::Bearing)},
     {"bearing-image-size", toUint8(Property::BearingImageSize)},
     {"location", toUint8(Property::Location)},
     {"shadow-image-size", toUint8(Property::ShadowImageSize)},
     {"top-image-size", toUint8(Property::TopImageSize)},
     {"accuracy-radius-transition", toUint8(Property::AccuracyRadiusTransition)},
     {"accuracy-radius-border-color-transition", toUint8(Property::AccuracyRadiusBorderColorTransition)},
     {"accuracy-radius-color-transition", toUint8(Property::AccuracyRadiusColorTransition)},
     {"bearing-transition", toUint8(Property::BearingTransition)},
     {"bearing-image-size-transition", toUint8(Property::BearingImageSizeTransition)},
     {"location-transition", toUint8(Property::LocationTransition)},
     {"shadow-image-size-transition", toUint8(Property::ShadowImageSizeTransition)},
     {"top-image-size-transition", toUint8(Property::TopImageSizeTransition)},
     {"bearing-image", toUint8(Property::BearingImage)},
     {"image-tilt-displacement", toUint8(Property::ImageTiltDisplacement)},
     {"perspective-compensation", toUint8(Property::PerspectiveCompensation)},
     {"shadow-image", toUint8(Property::ShadowImage)},
     {"top-image", toUint8(Property::TopImage)}});

StyleProperty getLayerProperty(const LocationIndicatorLayer& layer, Property property) {
    switch (property) {
        case Property::AccuracyRadius:
            return makeStyleProperty(layer.getAccuracyRadius());
        case Property::AccuracyRadiusBorderColor:
            return makeStyleProperty(layer.getAccuracyRadiusBorderColor());
        case Property::AccuracyRadiusColor:
            return makeStyleProperty(layer.getAccuracyRadiusColor());
        case Property::Bearing:
            return makeStyleProperty(layer.getBearing());
        case Property::BearingImageSize:
            return makeStyleProperty(layer.getBearingImageSize());
        case Property::Location:
            return makeStyleProperty(layer.getLocation());
        case Property::ShadowImageSize:
            return makeStyleProperty(layer.getShadowImageSize());
        case Property::TopImageSize:
            return makeStyleProperty(layer.getTopImageSize());
        case Property::AccuracyRadiusTransition:
            return makeStyleProperty(layer.getAccuracyRadiusTransition());
        case Property::AccuracyRadiusBorderColorTransition:
            return makeStyleProperty(layer.getAccuracyRadiusBorderColorTransition());
        case Property::AccuracyRadiusColorTransition:
            return makeStyleProperty(layer.getAccuracyRadiusColorTransition());
        case Property::BearingTransition:
            return makeStyleProperty(layer.getBearingTransition());
        case Property::BearingImageSizeTransition:
            return makeStyleProperty(layer.getBearingImageSizeTransition());
        case Property::LocationTransition:
            return makeStyleProperty(layer.getLocationTransition());
        case Property::ShadowImageSizeTransition:
            return makeStyleProperty(layer.getShadowImageSizeTransition());
        case Property::TopImageSizeTransition:
            return makeStyleProperty(layer.getTopImageSizeTransition());
        case Property::BearingImage:
            return makeStyleProperty(layer.getBearingImage());
        case Property::ImageTiltDisplacement:
            return makeStyleProperty(layer.getImageTiltDisplacement());
        case Property::PerspectiveCompensation:
            return makeStyleProperty(layer.getPerspectiveCompensation());
        case Property::ShadowImage:
            return makeStyleProperty(layer.getShadowImage());
        case Property::TopImage:
            return makeStyleProperty(layer.getTopImage());
    }
    return {};
}

StyleProperty getLayerProperty(const LocationIndicatorLayer& layer, const std::string& name) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }
    return getLayerProperty(layer, static_cast<Property>(it->second));
}

} // namespace

Value LocationIndicatorLayer::serialize() const {
    auto result = Layer::serialize();
    assert(result.getObject());
    for (const auto& property : layerProperties) {
        auto styleProperty = getLayerProperty(*this, static_cast<Property>(property.second));
        if (styleProperty.getKind() == StyleProperty::Kind::Undefined) continue;
        serializeProperty(result, styleProperty, property.first.c_str(), property.second < kPaintPropertyCount);
    }
    return result;
}

optional<Error> LocationIndicatorLayer::setPropertyInternal(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) return Error{"layer doesn't support this property"};

    auto property = static_cast<Property>(it->second);

    if (property == Property::AccuracyRadius || property == Property::BearingImageSize ||
        property == Property::ShadowImageSize || property == Property::TopImageSize ||
        property == Property::ImageTiltDisplacement || property == Property::PerspectiveCompensation) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::AccuracyRadius) {
            setAccuracyRadius(*typedValue);
            return nullopt;
        }

        if (property == Property::BearingImageSize) {
            setBearingImageSize(*typedValue);
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

        if (property == Property::ImageTiltDisplacement) {
            setImageTiltDisplacement(*typedValue);
            return nullopt;
        }

        if (property == Property::PerspectiveCompensation) {
            setPerspectiveCompensation(*typedValue);
            return nullopt;
        }
    }
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
    if (property == Property::Bearing) {
        Error error;
        const auto& typedValue = convert<PropertyValue<Rotation>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setBearing(*typedValue);
        return nullopt;
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

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }

    if (property == Property::AccuracyRadiusTransition) {
        setAccuracyRadiusTransition(*transition);
        return nullopt;
    }

    if (property == Property::AccuracyRadiusBorderColorTransition) {
        setAccuracyRadiusBorderColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::AccuracyRadiusColorTransition) {
        setAccuracyRadiusColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::BearingTransition) {
        setBearingTransition(*transition);
        return nullopt;
    }

    if (property == Property::BearingImageSizeTransition) {
        setBearingImageSizeTransition(*transition);
        return nullopt;
    }

    if (property == Property::LocationTransition) {
        setLocationTransition(*transition);
        return nullopt;
    }

    if (property == Property::ShadowImageSizeTransition) {
        setShadowImageSizeTransition(*transition);
        return nullopt;
    }

    if (property == Property::TopImageSizeTransition) {
        setTopImageSizeTransition(*transition);
        return nullopt;
    }

    return Error{"layer doesn't support this property"};
}

StyleProperty LocationIndicatorLayer::getProperty(const std::string& name) const {
    return getLayerProperty(*this, name);
}

Mutable<Layer::Impl> LocationIndicatorLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl

// clang-format on
