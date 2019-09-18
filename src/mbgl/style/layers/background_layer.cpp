// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
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
const LayerTypeInfo* BackgroundLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"background",
                                        LayerTypeInfo::Source::NotRequired,
                                        LayerTypeInfo::Pass3D::NotRequired,
                                        LayerTypeInfo::Layout::NotRequired,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::NotRequired};
    return &typeInfo;
}


BackgroundLayer::BackgroundLayer(const std::string& layerID)
    : Layer(makeMutable<Impl>(layerID, std::string())) {
}

BackgroundLayer::BackgroundLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

BackgroundLayer::~BackgroundLayer() = default;

const BackgroundLayer::Impl& BackgroundLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<BackgroundLayer::Impl> BackgroundLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> BackgroundLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = BackgroundPaintProperties::Transitionable();
    return std::make_unique<BackgroundLayer>(std::move(impl_));
}

void BackgroundLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>&) const {
}

// Layout properties


// Paint properties

PropertyValue<Color> BackgroundLayer::getDefaultBackgroundColor() {
    return { Color::black() };
}

const PropertyValue<Color>& BackgroundLayer::getBackgroundColor() const {
    return impl().paint.template get<BackgroundColor>().value;
}

void BackgroundLayer::setBackgroundColor(const PropertyValue<Color>& value) {
    if (value == getBackgroundColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundColorTransition() const {
    return impl().paint.template get<BackgroundColor>().options;
}

PropertyValue<float> BackgroundLayer::getDefaultBackgroundOpacity() {
    return { 1 };
}

const PropertyValue<float>& BackgroundLayer::getBackgroundOpacity() const {
    return impl().paint.template get<BackgroundOpacity>().value;
}

void BackgroundLayer::setBackgroundOpacity(const PropertyValue<float>& value) {
    if (value == getBackgroundOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundOpacityTransition() const {
    return impl().paint.template get<BackgroundOpacity>().options;
}

PropertyValue<std::string> BackgroundLayer::getDefaultBackgroundPattern() {
    return { "" };
}

const PropertyValue<std::string>& BackgroundLayer::getBackgroundPattern() const {
    return impl().paint.template get<BackgroundPattern>().value;
}

void BackgroundLayer::setBackgroundPattern(const PropertyValue<std::string>& value) {
    if (value == getBackgroundPattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void BackgroundLayer::setBackgroundPatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<BackgroundPattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions BackgroundLayer::getBackgroundPatternTransition() const {
    return impl().paint.template get<BackgroundPattern>().options;
}

using namespace conversion;

optional<Error> BackgroundLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
        BackgroundColor,
        BackgroundOpacity,
        BackgroundPattern,
        BackgroundColorTransition,
        BackgroundOpacityTransition,
        BackgroundPatternTransition,
    };

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "background-color", mbgl::underlying_type(Property::BackgroundColor) },
        { "background-opacity", mbgl::underlying_type(Property::BackgroundOpacity) },
        { "background-pattern", mbgl::underlying_type(Property::BackgroundPattern) },
        { "background-color-transition", mbgl::underlying_type(Property::BackgroundColorTransition) },
        { "background-opacity-transition", mbgl::underlying_type(Property::BackgroundOpacityTransition) },
        { "background-pattern-transition", mbgl::underlying_type(Property::BackgroundPatternTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);

        
    if (property == Property::BackgroundColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setBackgroundColor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::BackgroundOpacity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setBackgroundOpacity(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::BackgroundPattern) {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setBackgroundPattern(*typedValue);
        return nullopt;
        
    }
    

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }
    
    if (property == Property::BackgroundColorTransition) {
        setBackgroundColorTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::BackgroundOpacityTransition) {
        setBackgroundOpacityTransition(*transition);
        return nullopt;
    }
    
    if (property == Property::BackgroundPatternTransition) {
        setBackgroundPatternTransition(*transition);
        return nullopt;
    }
    

    return Error { "layer doesn't support this property" };
}

optional<Error> BackgroundLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        return Layer::setVisibility(value);
    }

    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> BackgroundLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
