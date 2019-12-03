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
    return {true};
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
    return {Color::black()};
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
    return {1};
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
    return {{}};
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

PropertyValue<expression::Image> FillLayer::getDefaultFillPattern() {
    return {{}};
}

const PropertyValue<expression::Image>& FillLayer::getFillPattern() const {
    return impl().paint.template get<FillPattern>().value;
}

void FillLayer::setFillPattern(const PropertyValue<expression::Image>& value) {
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
    return {{{0, 0}}};
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
    return {TranslateAnchorType::Map};
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

namespace {

enum class Property : uint8_t {
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

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"fill-antialias", toUint8(Property::FillAntialias)},
     {"fill-color", toUint8(Property::FillColor)},
     {"fill-opacity", toUint8(Property::FillOpacity)},
     {"fill-outline-color", toUint8(Property::FillOutlineColor)},
     {"fill-pattern", toUint8(Property::FillPattern)},
     {"fill-translate", toUint8(Property::FillTranslate)},
     {"fill-translate-anchor", toUint8(Property::FillTranslateAnchor)},
     {"fill-antialias-transition", toUint8(Property::FillAntialiasTransition)},
     {"fill-color-transition", toUint8(Property::FillColorTransition)},
     {"fill-opacity-transition", toUint8(Property::FillOpacityTransition)},
     {"fill-outline-color-transition", toUint8(Property::FillOutlineColorTransition)},
     {"fill-pattern-transition", toUint8(Property::FillPatternTransition)},
     {"fill-translate-transition", toUint8(Property::FillTranslateTransition)},
     {"fill-translate-anchor-transition", toUint8(Property::FillTranslateAnchorTransition)}});
} // namespace

optional<Error> FillLayer::setProperty(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        if (name == "visibility") return setVisibility(value);
        return Error{"layer doesn't support this property"};
    }

    auto property = static_cast<Property>(it->second);

    if (property == Property::FillAntialias) {
        Error error;
        const auto& typedValue = convert<PropertyValue<bool>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setFillAntialias(*typedValue);
        return nullopt;
    }
    if (property == Property::FillColor || property == Property::FillOutlineColor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<Color>>(value, error, true, false);
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
        const auto& typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setFillOpacity(*typedValue);
        return nullopt;
    }
    if (property == Property::FillPattern) {
        Error error;
        const auto& typedValue = convert<PropertyValue<expression::Image>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setFillPattern(*typedValue);
        return nullopt;
    }
    if (property == Property::FillTranslate) {
        Error error;
        const auto& typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setFillTranslate(*typedValue);
        return nullopt;
    }
    if (property == Property::FillTranslateAnchor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
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

    return Error{"layer doesn't support this property"};
}

StyleProperty FillLayer::getProperty(const std::string& name) const {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::FillAntialias:
            return makeStyleProperty(getFillAntialias());
        case Property::FillColor:
            return makeStyleProperty(getFillColor());
        case Property::FillOpacity:
            return makeStyleProperty(getFillOpacity());
        case Property::FillOutlineColor:
            return makeStyleProperty(getFillOutlineColor());
        case Property::FillPattern:
            return makeStyleProperty(getFillPattern());
        case Property::FillTranslate:
            return makeStyleProperty(getFillTranslate());
        case Property::FillTranslateAnchor:
            return makeStyleProperty(getFillTranslateAnchor());
        case Property::FillAntialiasTransition:
            return makeStyleProperty(getFillAntialiasTransition());
        case Property::FillColorTransition:
            return makeStyleProperty(getFillColorTransition());
        case Property::FillOpacityTransition:
            return makeStyleProperty(getFillOpacityTransition());
        case Property::FillOutlineColorTransition:
            return makeStyleProperty(getFillOutlineColorTransition());
        case Property::FillPatternTransition:
            return makeStyleProperty(getFillPatternTransition());
        case Property::FillTranslateTransition:
            return makeStyleProperty(getFillTranslateTransition());
        case Property::FillTranslateAnchorTransition:
            return makeStyleProperty(getFillTranslateAnchorTransition());
    }
    return {};
}

Mutable<Layer::Impl> FillLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
