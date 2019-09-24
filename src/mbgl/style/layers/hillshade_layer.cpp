// clang-format off

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
    return { Color::black() };
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
    return { 0.5 };
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
    return { Color::white() };
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
    return { HillshadeIlluminationAnchorType::Viewport };
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
    return { 335 };
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
    return { Color::black() };
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

enum class Property {
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

MAPBOX_ETERNAL_CONSTEXPR const auto paintProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"hillshade-accent-color", mbgl::underlying_type(Property::HillshadeAccentColor)},
     {"hillshade-exaggeration", mbgl::underlying_type(Property::HillshadeExaggeration)},
     {"hillshade-highlight-color", mbgl::underlying_type(Property::HillshadeHighlightColor)},
     {"hillshade-illumination-anchor", mbgl::underlying_type(Property::HillshadeIlluminationAnchor)},
     {"hillshade-illumination-direction", mbgl::underlying_type(Property::HillshadeIlluminationDirection)},
     {"hillshade-shadow-color", mbgl::underlying_type(Property::HillshadeShadowColor)},
     {"hillshade-accent-color-transition", mbgl::underlying_type(Property::HillshadeAccentColorTransition)},
     {"hillshade-exaggeration-transition", mbgl::underlying_type(Property::HillshadeExaggerationTransition)},
     {"hillshade-highlight-color-transition", mbgl::underlying_type(Property::HillshadeHighlightColorTransition)},
     {"hillshade-illumination-anchor-transition", mbgl::underlying_type(Property::HillshadeIlluminationAnchorTransition)},
     {"hillshade-illumination-direction-transition", mbgl::underlying_type(Property::HillshadeIlluminationDirectionTransition)},
     {"hillshade-shadow-color-transition", mbgl::underlying_type(Property::HillshadeShadowColorTransition)}});

} // namespace

optional<Error> HillshadeLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    const auto it = paintProperties.find(name.c_str());
    if (it == paintProperties.end()) {
        return Error { "layer doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);

        
    if (property == Property::HillshadeAccentColor || property == Property::HillshadeHighlightColor || property == Property::HillshadeShadowColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
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
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
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
        optional<PropertyValue<HillshadeIlluminationAnchorType>> typedValue = convert<PropertyValue<HillshadeIlluminationAnchorType>>(value, error, false, false);
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
    

    return Error { "layer doesn't support this property" };
}

LayerProperty HillshadeLayer::getPaintProperty(const std::string& name) const {
    const auto it = paintProperties.find(name.c_str());
    if (it == paintProperties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::HillshadeAccentColor:
            return conversion::makeLayerProperty(getHillshadeAccentColor());
        case Property::HillshadeExaggeration:
            return conversion::makeLayerProperty(getHillshadeExaggeration());
        case Property::HillshadeHighlightColor:
            return conversion::makeLayerProperty(getHillshadeHighlightColor());
        case Property::HillshadeIlluminationAnchor:
            return conversion::makeLayerProperty(getHillshadeIlluminationAnchor());
        case Property::HillshadeIlluminationDirection:
            return conversion::makeLayerProperty(getHillshadeIlluminationDirection());
        case Property::HillshadeShadowColor:
            return conversion::makeLayerProperty(getHillshadeShadowColor());
        case Property::HillshadeAccentColorTransition:
            return conversion::makeLayerProperty(getHillshadeAccentColorTransition());
        case Property::HillshadeExaggerationTransition:
            return conversion::makeLayerProperty(getHillshadeExaggerationTransition());
        case Property::HillshadeHighlightColorTransition:
            return conversion::makeLayerProperty(getHillshadeHighlightColorTransition());
        case Property::HillshadeIlluminationAnchorTransition:
            return conversion::makeLayerProperty(getHillshadeIlluminationAnchorTransition());
        case Property::HillshadeIlluminationDirectionTransition:
            return conversion::makeLayerProperty(getHillshadeIlluminationDirectionTransition());
        case Property::HillshadeShadowColorTransition:
            return conversion::makeLayerProperty(getHillshadeShadowColorTransition());
    }
    return {};
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

// clang-format on
