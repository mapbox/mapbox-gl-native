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
    return {Color::black()};
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
    return {1};
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

PropertyValue<expression::Image> BackgroundLayer::getDefaultBackgroundPattern() {
    return {{}};
}

const PropertyValue<expression::Image>& BackgroundLayer::getBackgroundPattern() const {
    return impl().paint.template get<BackgroundPattern>().value;
}

void BackgroundLayer::setBackgroundPattern(const PropertyValue<expression::Image>& value) {
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

namespace {

enum class Property : uint8_t {
    BackgroundColor,
    BackgroundOpacity,
    BackgroundPattern,
    BackgroundColorTransition,
    BackgroundOpacityTransition,
    BackgroundPatternTransition,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"background-color", toUint8(Property::BackgroundColor)},
     {"background-opacity", toUint8(Property::BackgroundOpacity)},
     {"background-pattern", toUint8(Property::BackgroundPattern)},
     {"background-color-transition", toUint8(Property::BackgroundColorTransition)},
     {"background-opacity-transition", toUint8(Property::BackgroundOpacityTransition)},
     {"background-pattern-transition", toUint8(Property::BackgroundPatternTransition)}});
} // namespace

optional<Error> BackgroundLayer::setProperty(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        if (name == "visibility") return setVisibility(value);
        return Error{"layer doesn't support this property"};
    }

    auto property = static_cast<Property>(it->second);

    if (property == Property::BackgroundColor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setBackgroundColor(*typedValue);
        return nullopt;
    }
    if (property == Property::BackgroundOpacity) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setBackgroundOpacity(*typedValue);
        return nullopt;
    }
    if (property == Property::BackgroundPattern) {
        Error error;
        const auto& typedValue = convert<PropertyValue<expression::Image>>(value, error, false, false);
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

    return Error{"layer doesn't support this property"};
}

StyleProperty BackgroundLayer::getProperty(const std::string& name) const {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::BackgroundColor:
            return makeStyleProperty(getBackgroundColor());
        case Property::BackgroundOpacity:
            return makeStyleProperty(getBackgroundOpacity());
        case Property::BackgroundPattern:
            return makeStyleProperty(getBackgroundPattern());
        case Property::BackgroundColorTransition:
            return makeStyleProperty(getBackgroundColorTransition());
        case Property::BackgroundOpacityTransition:
            return makeStyleProperty(getBackgroundOpacityTransition());
        case Property::BackgroundPatternTransition:
            return makeStyleProperty(getBackgroundPatternTransition());
    }
    return {};
}

Mutable<Layer::Impl> BackgroundLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
