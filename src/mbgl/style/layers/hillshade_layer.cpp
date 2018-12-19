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

#include <mapbox/eternal.hpp>

namespace mbgl {
namespace style {


// static
const LayerTypeInfo* HillshadeLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo
        {"hillshade",
          LayerTypeInfo::Source::Required,
          LayerTypeInfo::Pass3D::Required,
          LayerTypeInfo::Layout::NotRequired,
          LayerTypeInfo::Clipping::NotRequired
        };
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

PropertyValue<float> HillshadeLayer::getDefaultHillshadeIlluminationDirection() {
    return { 335 };
}

PropertyValue<float> HillshadeLayer::getHillshadeIlluminationDirection() const {
    return impl().paint.template get<HillshadeIlluminationDirection>().value;
}

void HillshadeLayer::setHillshadeIlluminationDirection(PropertyValue<float> value) {
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

PropertyValue<HillshadeIlluminationAnchorType> HillshadeLayer::getDefaultHillshadeIlluminationAnchor() {
    return { HillshadeIlluminationAnchorType::Viewport };
}

PropertyValue<HillshadeIlluminationAnchorType> HillshadeLayer::getHillshadeIlluminationAnchor() const {
    return impl().paint.template get<HillshadeIlluminationAnchor>().value;
}

void HillshadeLayer::setHillshadeIlluminationAnchor(PropertyValue<HillshadeIlluminationAnchorType> value) {
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

PropertyValue<float> HillshadeLayer::getDefaultHillshadeExaggeration() {
    return { 0.5 };
}

PropertyValue<float> HillshadeLayer::getHillshadeExaggeration() const {
    return impl().paint.template get<HillshadeExaggeration>().value;
}

void HillshadeLayer::setHillshadeExaggeration(PropertyValue<float> value) {
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

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeShadowColor() {
    return { Color::black() };
}

PropertyValue<Color> HillshadeLayer::getHillshadeShadowColor() const {
    return impl().paint.template get<HillshadeShadowColor>().value;
}

void HillshadeLayer::setHillshadeShadowColor(PropertyValue<Color> value) {
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

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeHighlightColor() {
    return { Color::white() };
}

PropertyValue<Color> HillshadeLayer::getHillshadeHighlightColor() const {
    return impl().paint.template get<HillshadeHighlightColor>().value;
}

void HillshadeLayer::setHillshadeHighlightColor(PropertyValue<Color> value) {
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

PropertyValue<Color> HillshadeLayer::getDefaultHillshadeAccentColor() {
    return { Color::black() };
}

PropertyValue<Color> HillshadeLayer::getHillshadeAccentColor() const {
    return impl().paint.template get<HillshadeAccentColor>().value;
}

void HillshadeLayer::setHillshadeAccentColor(PropertyValue<Color> value) {
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

using namespace conversion;

optional<Error> HillshadeLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property : uint8_t {
        HillshadeIlluminationDirection,
        HillshadeIlluminationAnchor,
        HillshadeExaggeration,
        HillshadeShadowColor,
        HillshadeHighlightColor,
        HillshadeAccentColor,
        HillshadeIlluminationDirectionTransition,
        HillshadeIlluminationAnchorTransition,
        HillshadeExaggerationTransition,
        HillshadeShadowColorTransition,
        HillshadeHighlightColorTransition,
        HillshadeAccentColorTransition,
    };

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "hillshade-illumination-direction", static_cast<uint8_t>(Property::HillshadeIlluminationDirection) },
        { "hillshade-illumination-anchor", static_cast<uint8_t>(Property::HillshadeIlluminationAnchor) },
        { "hillshade-exaggeration", static_cast<uint8_t>(Property::HillshadeExaggeration) },
        { "hillshade-shadow-color", static_cast<uint8_t>(Property::HillshadeShadowColor) },
        { "hillshade-highlight-color", static_cast<uint8_t>(Property::HillshadeHighlightColor) },
        { "hillshade-accent-color", static_cast<uint8_t>(Property::HillshadeAccentColor) },
        { "hillshade-illumination-direction-transition", static_cast<uint8_t>(Property::HillshadeIlluminationDirectionTransition) },
        { "hillshade-illumination-anchor-transition", static_cast<uint8_t>(Property::HillshadeIlluminationAnchorTransition) },
        { "hillshade-exaggeration-transition", static_cast<uint8_t>(Property::HillshadeExaggerationTransition) },
        { "hillshade-shadow-color-transition", static_cast<uint8_t>(Property::HillshadeShadowColorTransition) },
        { "hillshade-highlight-color-transition", static_cast<uint8_t>(Property::HillshadeHighlightColorTransition) },
        { "hillshade-accent-color-transition", static_cast<uint8_t>(Property::HillshadeAccentColorTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    Property property = static_cast<Property>(it->second);

        
    if (property == Property::HillshadeIlluminationDirection || property == Property::HillshadeExaggeration) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::HillshadeIlluminationDirection) {
            setHillshadeIlluminationDirection(*typedValue);
            return nullopt;
        }
        
        if (property == Property::HillshadeExaggeration) {
            setHillshadeExaggeration(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::HillshadeIlluminationAnchor) {
        Error error;
        optional<PropertyValue<HillshadeIlluminationAnchorType>> typedValue = convert<PropertyValue<HillshadeIlluminationAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setHillshadeIlluminationAnchor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::HillshadeShadowColor || property == Property::HillshadeHighlightColor || property == Property::HillshadeAccentColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::HillshadeShadowColor) {
            setHillshadeShadowColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::HillshadeHighlightColor) {
            setHillshadeHighlightColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::HillshadeAccentColor) {
            setHillshadeAccentColor(*typedValue);
            return nullopt;
        }
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::HillshadeIlluminationDirectionTransition) {
        setHillshadeIlluminationDirectionTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HillshadeIlluminationAnchorTransition) {
        setHillshadeIlluminationAnchorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HillshadeExaggerationTransition) {
        setHillshadeExaggerationTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HillshadeShadowColorTransition) {
        setHillshadeShadowColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HillshadeHighlightColorTransition) {
        setHillshadeHighlightColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::HillshadeAccentColorTransition) {
        setHillshadeAccentColorTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> HillshadeLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        return Layer::setVisibility(value);
    }

    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> HillshadeLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
