// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
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
const LayerTypeInfo* CircleLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"circle",
                                        LayerTypeInfo::Source::Required,
                                        LayerTypeInfo::Pass3D::NotRequired,
                                        LayerTypeInfo::Layout::NotRequired,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::Geometry};
    return &typeInfo;
}


CircleLayer::CircleLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
}

CircleLayer::CircleLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

CircleLayer::~CircleLayer() = default;

const CircleLayer::Impl& CircleLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<CircleLayer::Impl> CircleLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> CircleLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = CirclePaintProperties::Transitionable();
    return std::make_unique<CircleLayer>(std::move(impl_));
}

void CircleLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Layout properties


// Paint properties

PropertyValue<float> CircleLayer::getDefaultCircleBlur() {
    return {0};
}

const PropertyValue<float>& CircleLayer::getCircleBlur() const {
    return impl().paint.template get<CircleBlur>().value;
}

void CircleLayer::setCircleBlur(const PropertyValue<float>& value) {
    if (value == getCircleBlur())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleBlur>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleBlurTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleBlur>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleBlurTransition() const {
    return impl().paint.template get<CircleBlur>().options;
}

PropertyValue<Color> CircleLayer::getDefaultCircleColor() {
    return {Color::black()};
}

const PropertyValue<Color>& CircleLayer::getCircleColor() const {
    return impl().paint.template get<CircleColor>().value;
}

void CircleLayer::setCircleColor(const PropertyValue<Color>& value) {
    if (value == getCircleColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleColorTransition() const {
    return impl().paint.template get<CircleColor>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleOpacity() {
    return {1};
}

const PropertyValue<float>& CircleLayer::getCircleOpacity() const {
    return impl().paint.template get<CircleOpacity>().value;
}

void CircleLayer::setCircleOpacity(const PropertyValue<float>& value) {
    if (value == getCircleOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleOpacityTransition() const {
    return impl().paint.template get<CircleOpacity>().options;
}

PropertyValue<AlignmentType> CircleLayer::getDefaultCirclePitchAlignment() {
    return {AlignmentType::Viewport};
}

const PropertyValue<AlignmentType>& CircleLayer::getCirclePitchAlignment() const {
    return impl().paint.template get<CirclePitchAlignment>().value;
}

void CircleLayer::setCirclePitchAlignment(const PropertyValue<AlignmentType>& value) {
    if (value == getCirclePitchAlignment())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchAlignment>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCirclePitchAlignmentTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchAlignment>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCirclePitchAlignmentTransition() const {
    return impl().paint.template get<CirclePitchAlignment>().options;
}

PropertyValue<CirclePitchScaleType> CircleLayer::getDefaultCirclePitchScale() {
    return {CirclePitchScaleType::Map};
}

const PropertyValue<CirclePitchScaleType>& CircleLayer::getCirclePitchScale() const {
    return impl().paint.template get<CirclePitchScale>().value;
}

void CircleLayer::setCirclePitchScale(const PropertyValue<CirclePitchScaleType>& value) {
    if (value == getCirclePitchScale())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchScale>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCirclePitchScaleTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CirclePitchScale>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCirclePitchScaleTransition() const {
    return impl().paint.template get<CirclePitchScale>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleRadius() {
    return {5};
}

const PropertyValue<float>& CircleLayer::getCircleRadius() const {
    return impl().paint.template get<CircleRadius>().value;
}

void CircleLayer::setCircleRadius(const PropertyValue<float>& value) {
    if (value == getCircleRadius())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleRadius>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleRadiusTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleRadius>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleRadiusTransition() const {
    return impl().paint.template get<CircleRadius>().options;
}

PropertyValue<Color> CircleLayer::getDefaultCircleStrokeColor() {
    return {Color::black()};
}

const PropertyValue<Color>& CircleLayer::getCircleStrokeColor() const {
    return impl().paint.template get<CircleStrokeColor>().value;
}

void CircleLayer::setCircleStrokeColor(const PropertyValue<Color>& value) {
    if (value == getCircleStrokeColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeColorTransition() const {
    return impl().paint.template get<CircleStrokeColor>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleStrokeOpacity() {
    return {1};
}

const PropertyValue<float>& CircleLayer::getCircleStrokeOpacity() const {
    return impl().paint.template get<CircleStrokeOpacity>().value;
}

void CircleLayer::setCircleStrokeOpacity(const PropertyValue<float>& value) {
    if (value == getCircleStrokeOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeOpacityTransition() const {
    return impl().paint.template get<CircleStrokeOpacity>().options;
}

PropertyValue<float> CircleLayer::getDefaultCircleStrokeWidth() {
    return {0};
}

const PropertyValue<float>& CircleLayer::getCircleStrokeWidth() const {
    return impl().paint.template get<CircleStrokeWidth>().value;
}

void CircleLayer::setCircleStrokeWidth(const PropertyValue<float>& value) {
    if (value == getCircleStrokeWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleStrokeWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleStrokeWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleStrokeWidthTransition() const {
    return impl().paint.template get<CircleStrokeWidth>().options;
}

PropertyValue<std::array<float, 2>> CircleLayer::getDefaultCircleTranslate() {
    return {{{0, 0}}};
}

const PropertyValue<std::array<float, 2>>& CircleLayer::getCircleTranslate() const {
    return impl().paint.template get<CircleTranslate>().value;
}

void CircleLayer::setCircleTranslate(const PropertyValue<std::array<float, 2>>& value) {
    if (value == getCircleTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateTransition() const {
    return impl().paint.template get<CircleTranslate>().options;
}

PropertyValue<TranslateAnchorType> CircleLayer::getDefaultCircleTranslateAnchor() {
    return {TranslateAnchorType::Map};
}

const PropertyValue<TranslateAnchorType>& CircleLayer::getCircleTranslateAnchor() const {
    return impl().paint.template get<CircleTranslateAnchor>().value;
}

void CircleLayer::setCircleTranslateAnchor(const PropertyValue<TranslateAnchorType>& value) {
    if (value == getCircleTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void CircleLayer::setCircleTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<CircleTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions CircleLayer::getCircleTranslateAnchorTransition() const {
    return impl().paint.template get<CircleTranslateAnchor>().options;
}

using namespace conversion;

namespace {

enum class Property : uint8_t {
    CircleBlur,
    CircleColor,
    CircleOpacity,
    CirclePitchAlignment,
    CirclePitchScale,
    CircleRadius,
    CircleStrokeColor,
    CircleStrokeOpacity,
    CircleStrokeWidth,
    CircleTranslate,
    CircleTranslateAnchor,
    CircleBlurTransition,
    CircleColorTransition,
    CircleOpacityTransition,
    CirclePitchAlignmentTransition,
    CirclePitchScaleTransition,
    CircleRadiusTransition,
    CircleStrokeColorTransition,
    CircleStrokeOpacityTransition,
    CircleStrokeWidthTransition,
    CircleTranslateTransition,
    CircleTranslateAnchorTransition,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"circle-blur", toUint8(Property::CircleBlur)},
     {"circle-color", toUint8(Property::CircleColor)},
     {"circle-opacity", toUint8(Property::CircleOpacity)},
     {"circle-pitch-alignment", toUint8(Property::CirclePitchAlignment)},
     {"circle-pitch-scale", toUint8(Property::CirclePitchScale)},
     {"circle-radius", toUint8(Property::CircleRadius)},
     {"circle-stroke-color", toUint8(Property::CircleStrokeColor)},
     {"circle-stroke-opacity", toUint8(Property::CircleStrokeOpacity)},
     {"circle-stroke-width", toUint8(Property::CircleStrokeWidth)},
     {"circle-translate", toUint8(Property::CircleTranslate)},
     {"circle-translate-anchor", toUint8(Property::CircleTranslateAnchor)},
     {"circle-blur-transition", toUint8(Property::CircleBlurTransition)},
     {"circle-color-transition", toUint8(Property::CircleColorTransition)},
     {"circle-opacity-transition", toUint8(Property::CircleOpacityTransition)},
     {"circle-pitch-alignment-transition", toUint8(Property::CirclePitchAlignmentTransition)},
     {"circle-pitch-scale-transition", toUint8(Property::CirclePitchScaleTransition)},
     {"circle-radius-transition", toUint8(Property::CircleRadiusTransition)},
     {"circle-stroke-color-transition", toUint8(Property::CircleStrokeColorTransition)},
     {"circle-stroke-opacity-transition", toUint8(Property::CircleStrokeOpacityTransition)},
     {"circle-stroke-width-transition", toUint8(Property::CircleStrokeWidthTransition)},
     {"circle-translate-transition", toUint8(Property::CircleTranslateTransition)},
     {"circle-translate-anchor-transition", toUint8(Property::CircleTranslateAnchorTransition)}});
} // namespace

optional<Error> CircleLayer::setProperty(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        if (name == "visibility") return setVisibility(value);
        return Error{"layer doesn't support this property"};
    }

    auto property = static_cast<Property>(it->second);

    if (property == Property::CircleBlur || property == Property::CircleOpacity || property == Property::CircleRadius ||
        property == Property::CircleStrokeOpacity || property == Property::CircleStrokeWidth) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::CircleBlur) {
            setCircleBlur(*typedValue);
            return nullopt;
        }

        if (property == Property::CircleOpacity) {
            setCircleOpacity(*typedValue);
            return nullopt;
        }

        if (property == Property::CircleRadius) {
            setCircleRadius(*typedValue);
            return nullopt;
        }

        if (property == Property::CircleStrokeOpacity) {
            setCircleStrokeOpacity(*typedValue);
            return nullopt;
        }

        if (property == Property::CircleStrokeWidth) {
            setCircleStrokeWidth(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::CircleColor || property == Property::CircleStrokeColor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::CircleColor) {
            setCircleColor(*typedValue);
            return nullopt;
        }

        if (property == Property::CircleStrokeColor) {
            setCircleStrokeColor(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::CirclePitchAlignment) {
        Error error;
        const auto& typedValue = convert<PropertyValue<AlignmentType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCirclePitchAlignment(*typedValue);
        return nullopt;
    }
    if (property == Property::CirclePitchScale) {
        Error error;
        const auto& typedValue = convert<PropertyValue<CirclePitchScaleType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCirclePitchScale(*typedValue);
        return nullopt;
    }
    if (property == Property::CircleTranslate) {
        Error error;
        const auto& typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCircleTranslate(*typedValue);
        return nullopt;
    }
    if (property == Property::CircleTranslateAnchor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setCircleTranslateAnchor(*typedValue);
        return nullopt;
    }

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }

    if (property == Property::CircleBlurTransition) {
        setCircleBlurTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleColorTransition) {
        setCircleColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleOpacityTransition) {
        setCircleOpacityTransition(*transition);
        return nullopt;
    }

    if (property == Property::CirclePitchAlignmentTransition) {
        setCirclePitchAlignmentTransition(*transition);
        return nullopt;
    }

    if (property == Property::CirclePitchScaleTransition) {
        setCirclePitchScaleTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleRadiusTransition) {
        setCircleRadiusTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleStrokeColorTransition) {
        setCircleStrokeColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleStrokeOpacityTransition) {
        setCircleStrokeOpacityTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleStrokeWidthTransition) {
        setCircleStrokeWidthTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleTranslateTransition) {
        setCircleTranslateTransition(*transition);
        return nullopt;
    }

    if (property == Property::CircleTranslateAnchorTransition) {
        setCircleTranslateAnchorTransition(*transition);
        return nullopt;
    }

    return Error{"layer doesn't support this property"};
}

StyleProperty CircleLayer::getProperty(const std::string& name) const {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::CircleBlur:
            return makeStyleProperty(getCircleBlur());
        case Property::CircleColor:
            return makeStyleProperty(getCircleColor());
        case Property::CircleOpacity:
            return makeStyleProperty(getCircleOpacity());
        case Property::CirclePitchAlignment:
            return makeStyleProperty(getCirclePitchAlignment());
        case Property::CirclePitchScale:
            return makeStyleProperty(getCirclePitchScale());
        case Property::CircleRadius:
            return makeStyleProperty(getCircleRadius());
        case Property::CircleStrokeColor:
            return makeStyleProperty(getCircleStrokeColor());
        case Property::CircleStrokeOpacity:
            return makeStyleProperty(getCircleStrokeOpacity());
        case Property::CircleStrokeWidth:
            return makeStyleProperty(getCircleStrokeWidth());
        case Property::CircleTranslate:
            return makeStyleProperty(getCircleTranslate());
        case Property::CircleTranslateAnchor:
            return makeStyleProperty(getCircleTranslateAnchor());
        case Property::CircleBlurTransition:
            return makeStyleProperty(getCircleBlurTransition());
        case Property::CircleColorTransition:
            return makeStyleProperty(getCircleColorTransition());
        case Property::CircleOpacityTransition:
            return makeStyleProperty(getCircleOpacityTransition());
        case Property::CirclePitchAlignmentTransition:
            return makeStyleProperty(getCirclePitchAlignmentTransition());
        case Property::CirclePitchScaleTransition:
            return makeStyleProperty(getCirclePitchScaleTransition());
        case Property::CircleRadiusTransition:
            return makeStyleProperty(getCircleRadiusTransition());
        case Property::CircleStrokeColorTransition:
            return makeStyleProperty(getCircleStrokeColorTransition());
        case Property::CircleStrokeOpacityTransition:
            return makeStyleProperty(getCircleStrokeOpacityTransition());
        case Property::CircleStrokeWidthTransition:
            return makeStyleProperty(getCircleStrokeWidthTransition());
        case Property::CircleTranslateTransition:
            return makeStyleProperty(getCircleTranslateTransition());
        case Property::CircleTranslateAnchorTransition:
            return makeStyleProperty(getCircleTranslateAnchorTransition());
    }
    return {};
}

Mutable<Layer::Impl> CircleLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
