// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/hillshade_layer_impl.hpp>
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
const LayerTypeInfo* HillshadeLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"hillshade",
                                        LayerTypeInfo::Source::Required,
                                        LayerTypeInfo::Pass3D::Required,
                                        LayerTypeInfo::Layout::NotRequired,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::RasterDEM};
    return &typeInfo;
}


HillshadeLayer::HillshadeLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
}

HillshadeLayer::HillshadeLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

HillshadeLayer::~HillshadeLayer() = default;

const HillshadeLayer::Impl& HillshadeLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<HillshadeLayer::Impl> HillshadeLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> HillshadeLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = HillshadePaintProperties::Transitionable();
    return std::make_unique<HillshadeLayer>(std::move(impl_));
}

void HillshadeLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Layout properties


// Paint properties

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeAccentColor() {
    return {Color::black()};
}

const PropertyValue<Color>& HillshadeLayer::getHillshadeAccentColor() const {
    return impl().paint.template get<HillshadeAccentColor>().value;
}

void HillshadeLayer::setHillshadeAccentColor(const PropertyValue<Color>& value) {
    if (value == getHillshadeAccentColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeAccentColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeAccentColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeAccentColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeAccentColorTransition() const {
    return impl().paint.template get<HillshadeAccentColor>().options;
}

PropertyValue<float> HillshadeLayer::getDefaultHillshadeExaggeration() {
    return {0.5};
}

const PropertyValue<float>& HillshadeLayer::getHillshadeExaggeration() const {
    return impl().paint.template get<HillshadeExaggeration>().value;
}

void HillshadeLayer::setHillshadeExaggeration(const PropertyValue<float>& value) {
    if (value == getHillshadeExaggeration())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeExaggeration>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeExaggerationTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeExaggeration>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeExaggerationTransition() const {
    return impl().paint.template get<HillshadeExaggeration>().options;
}

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeHighlightColor() {
    return {Color::white()};
}

const PropertyValue<Color>& HillshadeLayer::getHillshadeHighlightColor() const {
    return impl().paint.template get<HillshadeHighlightColor>().value;
}

void HillshadeLayer::setHillshadeHighlightColor(const PropertyValue<Color>& value) {
    if (value == getHillshadeHighlightColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeHighlightColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeHighlightColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeHighlightColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeHighlightColorTransition() const {
    return impl().paint.template get<HillshadeHighlightColor>().options;
}

PropertyValue<HillshadeIlluminationAnchorType> HillshadeLayer::getDefaultHillshadeIlluminationAnchor() {
    return {HillshadeIlluminationAnchorType::Viewport};
}

const PropertyValue<HillshadeIlluminationAnchorType>& HillshadeLayer::getHillshadeIlluminationAnchor() const {
    return impl().paint.template get<HillshadeIlluminationAnchor>().value;
}

void HillshadeLayer::setHillshadeIlluminationAnchor(const PropertyValue<HillshadeIlluminationAnchorType>& value) {
    if (value == getHillshadeIlluminationAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeIlluminationAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeIlluminationAnchorTransition() const {
    return impl().paint.template get<HillshadeIlluminationAnchor>().options;
}

PropertyValue<float> HillshadeLayer::getDefaultHillshadeIlluminationDirection() {
    return {335};
}

const PropertyValue<float>& HillshadeLayer::getHillshadeIlluminationDirection() const {
    return impl().paint.template get<HillshadeIlluminationDirection>().value;
}

void HillshadeLayer::setHillshadeIlluminationDirection(const PropertyValue<float>& value) {
    if (value == getHillshadeIlluminationDirection())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationDirection>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeIlluminationDirectionTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeIlluminationDirection>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeIlluminationDirectionTransition() const {
    return impl().paint.template get<HillshadeIlluminationDirection>().options;
}

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeShadowColor() {
    return {Color::black()};
}

const PropertyValue<Color>& HillshadeLayer::getHillshadeShadowColor() const {
    return impl().paint.template get<HillshadeShadowColor>().value;
}

void HillshadeLayer::setHillshadeShadowColor(const PropertyValue<Color>& value) {
    if (value == getHillshadeShadowColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeShadowColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void HillshadeLayer::setHillshadeShadowColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<HillshadeShadowColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions HillshadeLayer::getHillshadeShadowColorTransition() const {
    return impl().paint.template get<HillshadeShadowColor>().options;
}

using namespace conversion;

namespace {

enum class Property : uint8_t {
    HillshadeAccentColor,
    HillshadeExaggeration,
    HillshadeHighlightColor,
    HillshadeIlluminationAnchor,
    HillshadeIlluminationDirection,
    HillshadeShadowColor,
    HillshadeAccentColorTransition,
    HillshadeExaggerationTransition,
    HillshadeHighlightColorTransition,
    HillshadeIlluminationAnchorTransition,
    HillshadeIlluminationDirectionTransition,
    HillshadeShadowColorTransition,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"hillshade-accent-color", toUint8(Property::HillshadeAccentColor)},
     {"hillshade-exaggeration", toUint8(Property::HillshadeExaggeration)},
     {"hillshade-highlight-color", toUint8(Property::HillshadeHighlightColor)},
     {"hillshade-illumination-anchor", toUint8(Property::HillshadeIlluminationAnchor)},
     {"hillshade-illumination-direction", toUint8(Property::HillshadeIlluminationDirection)},
     {"hillshade-shadow-color", toUint8(Property::HillshadeShadowColor)},
     {"hillshade-accent-color-transition", toUint8(Property::HillshadeAccentColorTransition)},
     {"hillshade-exaggeration-transition", toUint8(Property::HillshadeExaggerationTransition)},
     {"hillshade-highlight-color-transition", toUint8(Property::HillshadeHighlightColorTransition)},
     {"hillshade-illumination-anchor-transition", toUint8(Property::HillshadeIlluminationAnchorTransition)},
     {"hillshade-illumination-direction-transition", toUint8(Property::HillshadeIlluminationDirectionTransition)},
     {"hillshade-shadow-color-transition", toUint8(Property::HillshadeShadowColorTransition)}});
} // namespace

optional<Error> HillshadeLayer::setProperty(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        if (name == "visibility") return setVisibility(value);
        return Error{"layer doesn't support this property"};
    }

    auto property = static_cast<Property>(it->second);

    if (property == Property::HillshadeAccentColor || property == Property::HillshadeHighlightColor ||
        property == Property::HillshadeShadowColor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::HillshadeAccentColor) {
            setHillshadeAccentColor(*typedValue);
            return nullopt;
        }

        if (property == Property::HillshadeHighlightColor) {
            setHillshadeHighlightColor(*typedValue);
            return nullopt;
        }

        if (property == Property::HillshadeShadowColor) {
            setHillshadeShadowColor(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::HillshadeExaggeration || property == Property::HillshadeIlluminationDirection) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::HillshadeExaggeration) {
            setHillshadeExaggeration(*typedValue);
            return nullopt;
        }

        if (property == Property::HillshadeIlluminationDirection) {
            setHillshadeIlluminationDirection(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::HillshadeIlluminationAnchor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<HillshadeIlluminationAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setHillshadeIlluminationAnchor(*typedValue);
        return nullopt;
    }

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }

    if (property == Property::HillshadeAccentColorTransition) {
        setHillshadeAccentColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::HillshadeExaggerationTransition) {
        setHillshadeExaggerationTransition(*transition);
        return nullopt;
    }

    if (property == Property::HillshadeHighlightColorTransition) {
        setHillshadeHighlightColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::HillshadeIlluminationAnchorTransition) {
        setHillshadeIlluminationAnchorTransition(*transition);
        return nullopt;
    }

    if (property == Property::HillshadeIlluminationDirectionTransition) {
        setHillshadeIlluminationDirectionTransition(*transition);
        return nullopt;
    }

    if (property == Property::HillshadeShadowColorTransition) {
        setHillshadeShadowColorTransition(*transition);
        return nullopt;
    }

    return Error{"layer doesn't support this property"};
}

StyleProperty HillshadeLayer::getProperty(const std::string& name) const {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::HillshadeAccentColor:
            return makeStyleProperty(getHillshadeAccentColor());
        case Property::HillshadeExaggeration:
            return makeStyleProperty(getHillshadeExaggeration());
        case Property::HillshadeHighlightColor:
            return makeStyleProperty(getHillshadeHighlightColor());
        case Property::HillshadeIlluminationAnchor:
            return makeStyleProperty(getHillshadeIlluminationAnchor());
        case Property::HillshadeIlluminationDirection:
            return makeStyleProperty(getHillshadeIlluminationDirection());
        case Property::HillshadeShadowColor:
            return makeStyleProperty(getHillshadeShadowColor());
        case Property::HillshadeAccentColorTransition:
            return makeStyleProperty(getHillshadeAccentColorTransition());
        case Property::HillshadeExaggerationTransition:
            return makeStyleProperty(getHillshadeExaggerationTransition());
        case Property::HillshadeHighlightColorTransition:
            return makeStyleProperty(getHillshadeHighlightColorTransition());
        case Property::HillshadeIlluminationAnchorTransition:
            return makeStyleProperty(getHillshadeIlluminationAnchorTransition());
        case Property::HillshadeIlluminationDirectionTransition:
            return makeStyleProperty(getHillshadeIlluminationDirectionTransition());
        case Property::HillshadeShadowColorTransition:
            return makeStyleProperty(getHillshadeShadowColorTransition());
    }
    return {};
}

Mutable<Layer::Impl> HillshadeLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
