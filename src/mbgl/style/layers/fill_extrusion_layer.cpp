// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
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
const LayerTypeInfo* FillExtrusionLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo
        {"fill-extrusion",
          LayerTypeInfo::Source::Required,
          LayerTypeInfo::Pass3D::Required,
          LayerTypeInfo::Layout::Required,
          LayerTypeInfo::Clipping::NotRequired
        };
    return &typeInfo;
}


FillExtrusionLayer::FillExtrusionLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
}

FillExtrusionLayer::FillExtrusionLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

FillExtrusionLayer::~FillExtrusionLayer() = default;

const FillExtrusionLayer::Impl& FillExtrusionLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<FillExtrusionLayer::Impl> FillExtrusionLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> FillExtrusionLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = FillExtrusionPaintProperties::Transitionable();
    return std::make_unique<FillExtrusionLayer>(std::move(impl_));
}

void FillExtrusionLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Layout properties


// Paint properties

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionOpacity() {
    return { 1 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionOpacity() const {
    return impl().paint.template get<FillExtrusionOpacity>().value;
}

void FillExtrusionLayer::setFillExtrusionOpacity(PropertyValue<float> value) {
    if (value == getFillExtrusionOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionOpacityTransition() const {
    return impl().paint.template get<FillExtrusionOpacity>().options;
}

PropertyValue<Color> FillExtrusionLayer::getDefaultFillExtrusionColor() {
    return { Color::black() };
}

PropertyValue<Color> FillExtrusionLayer::getFillExtrusionColor() const {
    return impl().paint.template get<FillExtrusionColor>().value;
}

void FillExtrusionLayer::setFillExtrusionColor(PropertyValue<Color> value) {
    if (value == getFillExtrusionColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionColorTransition() const {
    return impl().paint.template get<FillExtrusionColor>().options;
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getDefaultFillExtrusionTranslate() {
    return { {{ 0, 0 }} };
}

PropertyValue<std::array<float, 2>> FillExtrusionLayer::getFillExtrusionTranslate() const {
    return impl().paint.template get<FillExtrusionTranslate>().value;
}

void FillExtrusionLayer::setFillExtrusionTranslate(PropertyValue<std::array<float, 2>> value) {
    if (value == getFillExtrusionTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateTransition() const {
    return impl().paint.template get<FillExtrusionTranslate>().options;
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getDefaultFillExtrusionTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

PropertyValue<TranslateAnchorType> FillExtrusionLayer::getFillExtrusionTranslateAnchor() const {
    return impl().paint.template get<FillExtrusionTranslateAnchor>().value;
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchor(PropertyValue<TranslateAnchorType> value) {
    if (value == getFillExtrusionTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionTranslateAnchorTransition() const {
    return impl().paint.template get<FillExtrusionTranslateAnchor>().options;
}

PropertyValue<std::string> FillExtrusionLayer::getDefaultFillExtrusionPattern() {
    return { "" };
}

PropertyValue<std::string> FillExtrusionLayer::getFillExtrusionPattern() const {
    return impl().paint.template get<FillExtrusionPattern>().value;
}

void FillExtrusionLayer::setFillExtrusionPattern(PropertyValue<std::string> value) {
    if (value == getFillExtrusionPattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionPattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionPatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionPattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionPatternTransition() const {
    return impl().paint.template get<FillExtrusionPattern>().options;
}

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionHeight() {
    return { 0 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionHeight() const {
    return impl().paint.template get<FillExtrusionHeight>().value;
}

void FillExtrusionLayer::setFillExtrusionHeight(PropertyValue<float> value) {
    if (value == getFillExtrusionHeight())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionHeight>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionHeightTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionHeight>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionHeightTransition() const {
    return impl().paint.template get<FillExtrusionHeight>().options;
}

PropertyValue<float> FillExtrusionLayer::getDefaultFillExtrusionBase() {
    return { 0 };
}

PropertyValue<float> FillExtrusionLayer::getFillExtrusionBase() const {
    return impl().paint.template get<FillExtrusionBase>().value;
}

void FillExtrusionLayer::setFillExtrusionBase(PropertyValue<float> value) {
    if (value == getFillExtrusionBase())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionBase>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionBaseTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionBase>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionBaseTransition() const {
    return impl().paint.template get<FillExtrusionBase>().options;
}

PropertyValue<bool> FillExtrusionLayer::getDefaultFillExtrusionVerticalGradient() {
    return { true };
}

PropertyValue<bool> FillExtrusionLayer::getFillExtrusionVerticalGradient() const {
    return impl().paint.template get<FillExtrusionVerticalGradient>().value;
}

void FillExtrusionLayer::setFillExtrusionVerticalGradient(PropertyValue<bool> value) {
    if (value == getFillExtrusionVerticalGradient())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionVerticalGradient>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillExtrusionLayer::setFillExtrusionVerticalGradientTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillExtrusionVerticalGradient>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillExtrusionLayer::getFillExtrusionVerticalGradientTransition() const {
    return impl().paint.template get<FillExtrusionVerticalGradient>().options;
}

using namespace conversion;

optional<Error> FillExtrusionLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property : uint8_t {
        FillExtrusionOpacity,
        FillExtrusionColor,
        FillExtrusionTranslate,
        FillExtrusionTranslateAnchor,
        FillExtrusionPattern,
        FillExtrusionHeight,
        FillExtrusionBase,
        FillExtrusionVerticalGradient,
        FillExtrusionOpacityTransition,
        FillExtrusionColorTransition,
        FillExtrusionTranslateTransition,
        FillExtrusionTranslateAnchorTransition,
        FillExtrusionPatternTransition,
        FillExtrusionHeightTransition,
        FillExtrusionBaseTransition,
        FillExtrusionVerticalGradientTransition,
    };

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "fill-extrusion-opacity", static_cast<uint8_t>(Property::FillExtrusionOpacity) },
        { "fill-extrusion-color", static_cast<uint8_t>(Property::FillExtrusionColor) },
        { "fill-extrusion-translate", static_cast<uint8_t>(Property::FillExtrusionTranslate) },
        { "fill-extrusion-translate-anchor", static_cast<uint8_t>(Property::FillExtrusionTranslateAnchor) },
        { "fill-extrusion-pattern", static_cast<uint8_t>(Property::FillExtrusionPattern) },
        { "fill-extrusion-height", static_cast<uint8_t>(Property::FillExtrusionHeight) },
        { "fill-extrusion-base", static_cast<uint8_t>(Property::FillExtrusionBase) },
        { "fill-extrusion-vertical-gradient", static_cast<uint8_t>(Property::FillExtrusionVerticalGradient) },
        { "fill-extrusion-opacity-transition", static_cast<uint8_t>(Property::FillExtrusionOpacityTransition) },
        { "fill-extrusion-color-transition", static_cast<uint8_t>(Property::FillExtrusionColorTransition) },
        { "fill-extrusion-translate-transition", static_cast<uint8_t>(Property::FillExtrusionTranslateTransition) },
        { "fill-extrusion-translate-anchor-transition", static_cast<uint8_t>(Property::FillExtrusionTranslateAnchorTransition) },
        { "fill-extrusion-pattern-transition", static_cast<uint8_t>(Property::FillExtrusionPatternTransition) },
        { "fill-extrusion-height-transition", static_cast<uint8_t>(Property::FillExtrusionHeightTransition) },
        { "fill-extrusion-base-transition", static_cast<uint8_t>(Property::FillExtrusionBaseTransition) },
        { "fill-extrusion-vertical-gradient-transition", static_cast<uint8_t>(Property::FillExtrusionVerticalGradientTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    Property property = static_cast<Property>(it->second);

        
    if (property == Property::FillExtrusionOpacity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillExtrusionOpacity(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillExtrusionColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setFillExtrusionColor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillExtrusionTranslate) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillExtrusionTranslate(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillExtrusionTranslateAnchor) {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillExtrusionTranslateAnchor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillExtrusionPattern) {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setFillExtrusionPattern(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillExtrusionHeight || property == Property::FillExtrusionBase) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::FillExtrusionHeight) {
            setFillExtrusionHeight(*typedValue);
            return nullopt;
        }
        
        if (property == Property::FillExtrusionBase) {
            setFillExtrusionBase(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::FillExtrusionVerticalGradient) {
        Error error;
        optional<PropertyValue<bool>> typedValue = convert<PropertyValue<bool>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillExtrusionVerticalGradient(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::FillExtrusionOpacityTransition) {
        setFillExtrusionOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillExtrusionColorTransition) {
        setFillExtrusionColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillExtrusionTranslateTransition) {
        setFillExtrusionTranslateTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillExtrusionTranslateAnchorTransition) {
        setFillExtrusionTranslateAnchorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillExtrusionPatternTransition) {
        setFillExtrusionPatternTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillExtrusionHeightTransition) {
        setFillExtrusionHeightTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillExtrusionBaseTransition) {
        setFillExtrusionBaseTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillExtrusionVerticalGradientTransition) {
        setFillExtrusionVerticalGradientTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> FillExtrusionLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        return Layer::setVisibility(value);
    }

    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> FillExtrusionLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
