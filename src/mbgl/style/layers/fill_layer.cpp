// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
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
const LayerTypeInfo* FillLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"fill",
                                        LayerTypeInfo::Source::Required,
                                        LayerTypeInfo::Pass3D::NotRequired,
                                        LayerTypeInfo::Layout::Required,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::Geometry};
    return &typeInfo;
}


FillLayer::FillLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
}

FillLayer::FillLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

FillLayer::~FillLayer() = default;

const FillLayer::Impl& FillLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<FillLayer::Impl> FillLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> FillLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = FillPaintProperties::Transitionable();
    return std::make_unique<FillLayer>(std::move(impl_));
}

void FillLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Layout properties


// Paint properties

PropertyValue<bool> FillLayer::getDefaultFillAntialias() {
    return { true };
}

const PropertyValue<bool>& FillLayer::getFillAntialias() const {
    return impl().paint.template get<FillAntialias>().value;
}

void FillLayer::setFillAntialias(const PropertyValue<bool>& value) {
    if (value == getFillAntialias())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillAntialias>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillAntialiasTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillAntialias>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillAntialiasTransition() const {
    return impl().paint.template get<FillAntialias>().options;
}

PropertyValue<Color> FillLayer::getDefaultFillColor() {
    return { Color::black() };
}

const PropertyValue<Color>& FillLayer::getFillColor() const {
    return impl().paint.template get<FillColor>().value;
}

void FillLayer::setFillColor(const PropertyValue<Color>& value) {
    if (value == getFillColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillColorTransition() const {
    return impl().paint.template get<FillColor>().options;
}

PropertyValue<float> FillLayer::getDefaultFillOpacity() {
    return { 1 };
}

const PropertyValue<float>& FillLayer::getFillOpacity() const {
    return impl().paint.template get<FillOpacity>().value;
}

void FillLayer::setFillOpacity(const PropertyValue<float>& value) {
    if (value == getFillOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillOpacityTransition() const {
    return impl().paint.template get<FillOpacity>().options;
}

PropertyValue<Color> FillLayer::getDefaultFillOutlineColor() {
    return { {} };
}

const PropertyValue<Color>& FillLayer::getFillOutlineColor() const {
    return impl().paint.template get<FillOutlineColor>().value;
}

void FillLayer::setFillOutlineColor(const PropertyValue<Color>& value) {
    if (value == getFillOutlineColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOutlineColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillOutlineColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillOutlineColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillOutlineColorTransition() const {
    return impl().paint.template get<FillOutlineColor>().options;
}

PropertyValue<std::string> FillLayer::getDefaultFillPattern() {
    return { "" };
}

const PropertyValue<std::string>& FillLayer::getFillPattern() const {
    return impl().paint.template get<FillPattern>().value;
}

void FillLayer::setFillPattern(const PropertyValue<std::string>& value) {
    if (value == getFillPattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillPattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillPatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillPattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillPatternTransition() const {
    return impl().paint.template get<FillPattern>().options;
}

PropertyValue<std::array<float, 2>> FillLayer::getDefaultFillTranslate() {
    return { {{ 0, 0 }} };
}

const PropertyValue<std::array<float, 2>>& FillLayer::getFillTranslate() const {
    return impl().paint.template get<FillTranslate>().value;
}

void FillLayer::setFillTranslate(const PropertyValue<std::array<float, 2>>& value) {
    if (value == getFillTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillTranslateTransition() const {
    return impl().paint.template get<FillTranslate>().options;
}

PropertyValue<TranslateAnchorType> FillLayer::getDefaultFillTranslateAnchor() {
    return { TranslateAnchorType::Map };
}

const PropertyValue<TranslateAnchorType>& FillLayer::getFillTranslateAnchor() const {
    return impl().paint.template get<FillTranslateAnchor>().value;
}

void FillLayer::setFillTranslateAnchor(const PropertyValue<TranslateAnchorType>& value) {
    if (value == getFillTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void FillLayer::setFillTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<FillTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions FillLayer::getFillTranslateAnchorTransition() const {
    return impl().paint.template get<FillTranslateAnchor>().options;
}

using namespace conversion;

optional<Error> FillLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
        FillAntialias,
        FillColor,
        FillOpacity,
        FillOutlineColor,
        FillPattern,
        FillTranslate,
        FillTranslateAnchor,
        FillAntialiasTransition,
        FillColorTransition,
        FillOpacityTransition,
        FillOutlineColorTransition,
        FillPatternTransition,
        FillTranslateTransition,
        FillTranslateAnchorTransition,
    };

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "fill-antialias", mbgl::underlying_type(Property::FillAntialias) },
        { "fill-color", mbgl::underlying_type(Property::FillColor) },
        { "fill-opacity", mbgl::underlying_type(Property::FillOpacity) },
        { "fill-outline-color", mbgl::underlying_type(Property::FillOutlineColor) },
        { "fill-pattern", mbgl::underlying_type(Property::FillPattern) },
        { "fill-translate", mbgl::underlying_type(Property::FillTranslate) },
        { "fill-translate-anchor", mbgl::underlying_type(Property::FillTranslateAnchor) },
        { "fill-antialias-transition", mbgl::underlying_type(Property::FillAntialiasTransition) },
        { "fill-color-transition", mbgl::underlying_type(Property::FillColorTransition) },
        { "fill-opacity-transition", mbgl::underlying_type(Property::FillOpacityTransition) },
        { "fill-outline-color-transition", mbgl::underlying_type(Property::FillOutlineColorTransition) },
        { "fill-pattern-transition", mbgl::underlying_type(Property::FillPatternTransition) },
        { "fill-translate-transition", mbgl::underlying_type(Property::FillTranslateTransition) },
        { "fill-translate-anchor-transition", mbgl::underlying_type(Property::FillTranslateAnchorTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);

        
    if (property == Property::FillAntialias) {
        Error error;
        optional<PropertyValue<bool>> typedValue = convert<PropertyValue<bool>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillAntialias(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillColor || property == Property::FillOutlineColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        if (property == Property::FillColor) {
            setFillColor(*typedValue);
            return nullopt;
        }
        
        if (property == Property::FillOutlineColor) {
            setFillOutlineColor(*typedValue);
            return nullopt;
        }
        
    }
    
    if (property == Property::FillOpacity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setFillOpacity(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillPattern) {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setFillPattern(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillTranslate) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillTranslate(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::FillTranslateAnchor) {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setFillTranslateAnchor(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::FillAntialiasTransition) {
        setFillAntialiasTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillColorTransition) {
        setFillColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillOpacityTransition) {
        setFillOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillOutlineColorTransition) {
        setFillOutlineColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillPatternTransition) {
        setFillPatternTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillTranslateTransition) {
        setFillTranslateTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::FillTranslateAnchorTransition) {
        setFillTranslateAnchorTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> FillLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        return Layer::setVisibility(value);
    }

    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> FillLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl

// clang-format on
