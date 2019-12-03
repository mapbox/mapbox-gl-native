// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
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
const LayerTypeInfo* RasterLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"raster",
                                        LayerTypeInfo::Source::Required,
                                        LayerTypeInfo::Pass3D::NotRequired,
                                        LayerTypeInfo::Layout::NotRequired,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::Raster};
    return &typeInfo;
}


RasterLayer::RasterLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
}

RasterLayer::RasterLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

RasterLayer::~RasterLayer() = default;

const RasterLayer::Impl& RasterLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<RasterLayer::Impl> RasterLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> RasterLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = RasterPaintProperties::Transitionable();
    return std::make_unique<RasterLayer>(std::move(impl_));
}

void RasterLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Layout properties


// Paint properties

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMax() {
    return {1};
}

const PropertyValue<float>& RasterLayer::getRasterBrightnessMax() const {
    return impl().paint.template get<RasterBrightnessMax>().value;
}

void RasterLayer::setRasterBrightnessMax(const PropertyValue<float>& value) {
    if (value == getRasterBrightnessMax())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterBrightnessMaxTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMax>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMaxTransition() const {
    return impl().paint.template get<RasterBrightnessMax>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterBrightnessMin() {
    return {0};
}

const PropertyValue<float>& RasterLayer::getRasterBrightnessMin() const {
    return impl().paint.template get<RasterBrightnessMin>().value;
}

void RasterLayer::setRasterBrightnessMin(const PropertyValue<float>& value) {
    if (value == getRasterBrightnessMin())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterBrightnessMinTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterBrightnessMin>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterBrightnessMinTransition() const {
    return impl().paint.template get<RasterBrightnessMin>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterContrast() {
    return {0};
}

const PropertyValue<float>& RasterLayer::getRasterContrast() const {
    return impl().paint.template get<RasterContrast>().value;
}

void RasterLayer::setRasterContrast(const PropertyValue<float>& value) {
    if (value == getRasterContrast())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterContrastTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterContrast>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterContrastTransition() const {
    return impl().paint.template get<RasterContrast>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterFadeDuration() {
    return {300};
}

const PropertyValue<float>& RasterLayer::getRasterFadeDuration() const {
    return impl().paint.template get<RasterFadeDuration>().value;
}

void RasterLayer::setRasterFadeDuration(const PropertyValue<float>& value) {
    if (value == getRasterFadeDuration())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterFadeDurationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterFadeDuration>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterFadeDurationTransition() const {
    return impl().paint.template get<RasterFadeDuration>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterHueRotate() {
    return {0};
}

const PropertyValue<float>& RasterLayer::getRasterHueRotate() const {
    return impl().paint.template get<RasterHueRotate>().value;
}

void RasterLayer::setRasterHueRotate(const PropertyValue<float>& value) {
    if (value == getRasterHueRotate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterHueRotateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterHueRotate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterHueRotateTransition() const {
    return impl().paint.template get<RasterHueRotate>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterOpacity() {
    return {1};
}

const PropertyValue<float>& RasterLayer::getRasterOpacity() const {
    return impl().paint.template get<RasterOpacity>().value;
}

void RasterLayer::setRasterOpacity(const PropertyValue<float>& value) {
    if (value == getRasterOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterOpacityTransition() const {
    return impl().paint.template get<RasterOpacity>().options;
}

PropertyValue<RasterResamplingType> RasterLayer::getDefaultRasterResampling() {
    return {RasterResamplingType::Linear};
}

const PropertyValue<RasterResamplingType>& RasterLayer::getRasterResampling() const {
    return impl().paint.template get<RasterResampling>().value;
}

void RasterLayer::setRasterResampling(const PropertyValue<RasterResamplingType>& value) {
    if (value == getRasterResampling())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterResampling>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterResamplingTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterResampling>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterResamplingTransition() const {
    return impl().paint.template get<RasterResampling>().options;
}

PropertyValue<float> RasterLayer::getDefaultRasterSaturation() {
    return {0};
}

const PropertyValue<float>& RasterLayer::getRasterSaturation() const {
    return impl().paint.template get<RasterSaturation>().value;
}

void RasterLayer::setRasterSaturation(const PropertyValue<float>& value) {
    if (value == getRasterSaturation())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void RasterLayer::setRasterSaturationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<RasterSaturation>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions RasterLayer::getRasterSaturationTransition() const {
    return impl().paint.template get<RasterSaturation>().options;
}

using namespace conversion;

namespace {

enum class Property : uint8_t {
    RasterBrightnessMax,
    RasterBrightnessMin,
    RasterContrast,
    RasterFadeDuration,
    RasterHueRotate,
    RasterOpacity,
    RasterResampling,
    RasterSaturation,
    RasterBrightnessMaxTransition,
    RasterBrightnessMinTransition,
    RasterContrastTransition,
    RasterFadeDurationTransition,
    RasterHueRotateTransition,
    RasterOpacityTransition,
    RasterResamplingTransition,
    RasterSaturationTransition,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"raster-brightness-max", toUint8(Property::RasterBrightnessMax)},
     {"raster-brightness-min", toUint8(Property::RasterBrightnessMin)},
     {"raster-contrast", toUint8(Property::RasterContrast)},
     {"raster-fade-duration", toUint8(Property::RasterFadeDuration)},
     {"raster-hue-rotate", toUint8(Property::RasterHueRotate)},
     {"raster-opacity", toUint8(Property::RasterOpacity)},
     {"raster-resampling", toUint8(Property::RasterResampling)},
     {"raster-saturation", toUint8(Property::RasterSaturation)},
     {"raster-brightness-max-transition", toUint8(Property::RasterBrightnessMaxTransition)},
     {"raster-brightness-min-transition", toUint8(Property::RasterBrightnessMinTransition)},
     {"raster-contrast-transition", toUint8(Property::RasterContrastTransition)},
     {"raster-fade-duration-transition", toUint8(Property::RasterFadeDurationTransition)},
     {"raster-hue-rotate-transition", toUint8(Property::RasterHueRotateTransition)},
     {"raster-opacity-transition", toUint8(Property::RasterOpacityTransition)},
     {"raster-resampling-transition", toUint8(Property::RasterResamplingTransition)},
     {"raster-saturation-transition", toUint8(Property::RasterSaturationTransition)}});
} // namespace

optional<Error> RasterLayer::setProperty(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        if (name == "visibility") return setVisibility(value);
        return Error{"layer doesn't support this property"};
    }

    auto property = static_cast<Property>(it->second);

    if (property == Property::RasterBrightnessMax || property == Property::RasterBrightnessMin ||
        property == Property::RasterContrast || property == Property::RasterFadeDuration ||
        property == Property::RasterHueRotate || property == Property::RasterOpacity ||
        property == Property::RasterSaturation) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::RasterBrightnessMax) {
            setRasterBrightnessMax(*typedValue);
            return nullopt;
        }

        if (property == Property::RasterBrightnessMin) {
            setRasterBrightnessMin(*typedValue);
            return nullopt;
        }

        if (property == Property::RasterContrast) {
            setRasterContrast(*typedValue);
            return nullopt;
        }

        if (property == Property::RasterFadeDuration) {
            setRasterFadeDuration(*typedValue);
            return nullopt;
        }

        if (property == Property::RasterHueRotate) {
            setRasterHueRotate(*typedValue);
            return nullopt;
        }

        if (property == Property::RasterOpacity) {
            setRasterOpacity(*typedValue);
            return nullopt;
        }

        if (property == Property::RasterSaturation) {
            setRasterSaturation(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::RasterResampling) {
        Error error;
        const auto& typedValue = convert<PropertyValue<RasterResamplingType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setRasterResampling(*typedValue);
        return nullopt;
    }

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }

    if (property == Property::RasterBrightnessMaxTransition) {
        setRasterBrightnessMaxTransition(*transition);
        return nullopt;
    }

    if (property == Property::RasterBrightnessMinTransition) {
        setRasterBrightnessMinTransition(*transition);
        return nullopt;
    }

    if (property == Property::RasterContrastTransition) {
        setRasterContrastTransition(*transition);
        return nullopt;
    }

    if (property == Property::RasterFadeDurationTransition) {
        setRasterFadeDurationTransition(*transition);
        return nullopt;
    }

    if (property == Property::RasterHueRotateTransition) {
        setRasterHueRotateTransition(*transition);
        return nullopt;
    }

    if (property == Property::RasterOpacityTransition) {
        setRasterOpacityTransition(*transition);
        return nullopt;
    }

    if (property == Property::RasterResamplingTransition) {
        setRasterResamplingTransition(*transition);
        return nullopt;
    }

    if (property == Property::RasterSaturationTransition) {
        setRasterSaturationTransition(*transition);
        return nullopt;
    }

    return Error{"layer doesn't support this property"};
}

StyleProperty RasterLayer::getProperty(const std::string& name) const {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::RasterBrightnessMax:
            return makeStyleProperty(getRasterBrightnessMax());
        case Property::RasterBrightnessMin:
            return makeStyleProperty(getRasterBrightnessMin());
        case Property::RasterContrast:
            return makeStyleProperty(getRasterContrast());
        case Property::RasterFadeDuration:
            return makeStyleProperty(getRasterFadeDuration());
        case Property::RasterHueRotate:
            return makeStyleProperty(getRasterHueRotate());
        case Property::RasterOpacity:
            return makeStyleProperty(getRasterOpacity());
        case Property::RasterResampling:
            return makeStyleProperty(getRasterResampling());
        case Property::RasterSaturation:
            return makeStyleProperty(getRasterSaturation());
        case Property::RasterBrightnessMaxTransition:
            return makeStyleProperty(getRasterBrightnessMaxTransition());
        case Property::RasterBrightnessMinTransition:
            return makeStyleProperty(getRasterBrightnessMinTransition());
        case Property::RasterContrastTransition:
            return makeStyleProperty(getRasterContrastTransition());
        case Property::RasterFadeDurationTransition:
            return makeStyleProperty(getRasterFadeDurationTransition());
        case Property::RasterHueRotateTransition:
            return makeStyleProperty(getRasterHueRotateTransition());
        case Property::RasterOpacityTransition:
            return makeStyleProperty(getRasterOpacityTransition());
        case Property::RasterResamplingTransition:
            return makeStyleProperty(getRasterResamplingTransition());
        case Property::RasterSaturationTransition:
            return makeStyleProperty(getRasterSaturationTransition());
    }
    return {};
}

Mutable<Layer::Impl> RasterLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
