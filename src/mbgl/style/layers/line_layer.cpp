// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
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
const LayerTypeInfo* LineLayer::Impl::staticTypeInfo() noexcept {
    const static LayerTypeInfo typeInfo{"line",
                                        LayerTypeInfo::Source::Required,
                                        LayerTypeInfo::Pass3D::NotRequired,
                                        LayerTypeInfo::Layout::Required,
                                        LayerTypeInfo::FadingTiles::NotRequired,
                                        LayerTypeInfo::CrossTileIndex::NotRequired,
                                        LayerTypeInfo::TileKind::Geometry};
    return &typeInfo;
}


LineLayer::LineLayer(const std::string& layerID, const std::string& sourceID)
    : Layer(makeMutable<Impl>(layerID, sourceID)) {
}

LineLayer::LineLayer(Immutable<Impl> impl_)
    : Layer(std::move(impl_)) {
}

LineLayer::~LineLayer() = default;

const LineLayer::Impl& LineLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<LineLayer::Impl> LineLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> LineLayer::cloneRef(const std::string& id_) const {
    auto impl_ = mutableImpl();
    impl_->id = id_;
    impl_->paint = LinePaintProperties::Transitionable();
    return std::make_unique<LineLayer>(std::move(impl_));
}

void LineLayer::Impl::stringifyLayout(rapidjson::Writer<rapidjson::StringBuffer>& writer) const {
    layout.stringify(writer);
}

// Layout properties

PropertyValue<LineCapType> LineLayer::getDefaultLineCap() {
    return LineCap::defaultValue();
}

const PropertyValue<LineCapType>& LineLayer::getLineCap() const {
    return impl().layout.get<LineCap>();
}

void LineLayer::setLineCap(const PropertyValue<LineCapType>& value) {
    if (value == getLineCap())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineCap>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<LineJoinType> LineLayer::getDefaultLineJoin() {
    return LineJoin::defaultValue();
}

const PropertyValue<LineJoinType>& LineLayer::getLineJoin() const {
    return impl().layout.get<LineJoin>();
}

void LineLayer::setLineJoin(const PropertyValue<LineJoinType>& value) {
    if (value == getLineJoin())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineJoin>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LineLayer::getDefaultLineMiterLimit() {
    return LineMiterLimit::defaultValue();
}

const PropertyValue<float>& LineLayer::getLineMiterLimit() const {
    return impl().layout.get<LineMiterLimit>();
}

void LineLayer::setLineMiterLimit(const PropertyValue<float>& value) {
    if (value == getLineMiterLimit())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineMiterLimit>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}
PropertyValue<float> LineLayer::getDefaultLineRoundLimit() {
    return LineRoundLimit::defaultValue();
}

const PropertyValue<float>& LineLayer::getLineRoundLimit() const {
    return impl().layout.get<LineRoundLimit>();
}

void LineLayer::setLineRoundLimit(const PropertyValue<float>& value) {
    if (value == getLineRoundLimit())
        return;
    auto impl_ = mutableImpl();
    impl_->layout.get<LineRoundLimit>() = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

// Paint properties

PropertyValue<float> LineLayer::getDefaultLineBlur() {
    return {0};
}

const PropertyValue<float>& LineLayer::getLineBlur() const {
    return impl().paint.template get<LineBlur>().value;
}

void LineLayer::setLineBlur(const PropertyValue<float>& value) {
    if (value == getLineBlur())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineBlur>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineBlurTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineBlur>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineBlurTransition() const {
    return impl().paint.template get<LineBlur>().options;
}

PropertyValue<Color> LineLayer::getDefaultLineColor() {
    return {Color::black()};
}

const PropertyValue<Color>& LineLayer::getLineColor() const {
    return impl().paint.template get<LineColor>().value;
}

void LineLayer::setLineColor(const PropertyValue<Color>& value) {
    if (value == getLineColor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineColor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineColor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineColorTransition() const {
    return impl().paint.template get<LineColor>().options;
}

PropertyValue<std::vector<float>> LineLayer::getDefaultLineDasharray() {
    return {{}};
}

const PropertyValue<std::vector<float>>& LineLayer::getLineDasharray() const {
    return impl().paint.template get<LineDasharray>().value;
}

void LineLayer::setLineDasharray(const PropertyValue<std::vector<float>>& value) {
    if (value == getLineDasharray())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineDasharray>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineDasharrayTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineDasharray>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineDasharrayTransition() const {
    return impl().paint.template get<LineDasharray>().options;
}

PropertyValue<float> LineLayer::getDefaultLineGapWidth() {
    return {0};
}

const PropertyValue<float>& LineLayer::getLineGapWidth() const {
    return impl().paint.template get<LineGapWidth>().value;
}

void LineLayer::setLineGapWidth(const PropertyValue<float>& value) {
    if (value == getLineGapWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGapWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineGapWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGapWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineGapWidthTransition() const {
    return impl().paint.template get<LineGapWidth>().options;
}

ColorRampPropertyValue LineLayer::getDefaultLineGradient() {
    return {{}};
}

const ColorRampPropertyValue& LineLayer::getLineGradient() const {
    return impl().paint.template get<LineGradient>().value;
}

void LineLayer::setLineGradient(const ColorRampPropertyValue& value) {
    if (value == getLineGradient())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGradient>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineGradientTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineGradient>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineGradientTransition() const {
    return impl().paint.template get<LineGradient>().options;
}

PropertyValue<float> LineLayer::getDefaultLineOffset() {
    return {0};
}

const PropertyValue<float>& LineLayer::getLineOffset() const {
    return impl().paint.template get<LineOffset>().value;
}

void LineLayer::setLineOffset(const PropertyValue<float>& value) {
    if (value == getLineOffset())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOffset>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineOffsetTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOffset>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineOffsetTransition() const {
    return impl().paint.template get<LineOffset>().options;
}

PropertyValue<float> LineLayer::getDefaultLineOpacity() {
    return {1};
}

const PropertyValue<float>& LineLayer::getLineOpacity() const {
    return impl().paint.template get<LineOpacity>().value;
}

void LineLayer::setLineOpacity(const PropertyValue<float>& value) {
    if (value == getLineOpacity())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOpacity>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineOpacityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineOpacity>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineOpacityTransition() const {
    return impl().paint.template get<LineOpacity>().options;
}

PropertyValue<expression::Image> LineLayer::getDefaultLinePattern() {
    return {{}};
}

const PropertyValue<expression::Image>& LineLayer::getLinePattern() const {
    return impl().paint.template get<LinePattern>().value;
}

void LineLayer::setLinePattern(const PropertyValue<expression::Image>& value) {
    if (value == getLinePattern())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LinePattern>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLinePatternTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LinePattern>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLinePatternTransition() const {
    return impl().paint.template get<LinePattern>().options;
}

PropertyValue<std::array<float, 2>> LineLayer::getDefaultLineTranslate() {
    return {{{0, 0}}};
}

const PropertyValue<std::array<float, 2>>& LineLayer::getLineTranslate() const {
    return impl().paint.template get<LineTranslate>().value;
}

void LineLayer::setLineTranslate(const PropertyValue<std::array<float, 2>>& value) {
    if (value == getLineTranslate())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslate>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineTranslateTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslate>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineTranslateTransition() const {
    return impl().paint.template get<LineTranslate>().options;
}

PropertyValue<TranslateAnchorType> LineLayer::getDefaultLineTranslateAnchor() {
    return {TranslateAnchorType::Map};
}

const PropertyValue<TranslateAnchorType>& LineLayer::getLineTranslateAnchor() const {
    return impl().paint.template get<LineTranslateAnchor>().value;
}

void LineLayer::setLineTranslateAnchor(const PropertyValue<TranslateAnchorType>& value) {
    if (value == getLineTranslateAnchor())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslateAnchor>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineTranslateAnchorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineTranslateAnchor>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineTranslateAnchorTransition() const {
    return impl().paint.template get<LineTranslateAnchor>().options;
}

PropertyValue<float> LineLayer::getDefaultLineWidth() {
    return {1};
}

const PropertyValue<float>& LineLayer::getLineWidth() const {
    return impl().paint.template get<LineWidth>().value;
}

void LineLayer::setLineWidth(const PropertyValue<float>& value) {
    if (value == getLineWidth())
        return;
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineWidth>().value = value;
    impl_->paint.template get<LineFloorWidth>().value = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void LineLayer::setLineWidthTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->paint.template get<LineWidth>().options = options;
    baseImpl = std::move(impl_);
}

TransitionOptions LineLayer::getLineWidthTransition() const {
    return impl().paint.template get<LineWidth>().options;
}

using namespace conversion;

namespace {

enum class Property : uint8_t {
    LineBlur,
    LineColor,
    LineDasharray,
    LineGapWidth,
    LineGradient,
    LineOffset,
    LineOpacity,
    LinePattern,
    LineTranslate,
    LineTranslateAnchor,
    LineWidth,
    LineBlurTransition,
    LineColorTransition,
    LineDasharrayTransition,
    LineGapWidthTransition,
    LineGradientTransition,
    LineOffsetTransition,
    LineOpacityTransition,
    LinePatternTransition,
    LineTranslateTransition,
    LineTranslateAnchorTransition,
    LineWidthTransition,
    LineCap,
    LineJoin,
    LineMiterLimit,
    LineRoundLimit,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"line-blur", toUint8(Property::LineBlur)},
     {"line-color", toUint8(Property::LineColor)},
     {"line-dasharray", toUint8(Property::LineDasharray)},
     {"line-gap-width", toUint8(Property::LineGapWidth)},
     {"line-gradient", toUint8(Property::LineGradient)},
     {"line-offset", toUint8(Property::LineOffset)},
     {"line-opacity", toUint8(Property::LineOpacity)},
     {"line-pattern", toUint8(Property::LinePattern)},
     {"line-translate", toUint8(Property::LineTranslate)},
     {"line-translate-anchor", toUint8(Property::LineTranslateAnchor)},
     {"line-width", toUint8(Property::LineWidth)},
     {"line-blur-transition", toUint8(Property::LineBlurTransition)},
     {"line-color-transition", toUint8(Property::LineColorTransition)},
     {"line-dasharray-transition", toUint8(Property::LineDasharrayTransition)},
     {"line-gap-width-transition", toUint8(Property::LineGapWidthTransition)},
     {"line-gradient-transition", toUint8(Property::LineGradientTransition)},
     {"line-offset-transition", toUint8(Property::LineOffsetTransition)},
     {"line-opacity-transition", toUint8(Property::LineOpacityTransition)},
     {"line-pattern-transition", toUint8(Property::LinePatternTransition)},
     {"line-translate-transition", toUint8(Property::LineTranslateTransition)},
     {"line-translate-anchor-transition", toUint8(Property::LineTranslateAnchorTransition)},
     {"line-width-transition", toUint8(Property::LineWidthTransition)},
     {"line-cap", toUint8(Property::LineCap)},
     {"line-join", toUint8(Property::LineJoin)},
     {"line-miter-limit", toUint8(Property::LineMiterLimit)},
     {"line-round-limit", toUint8(Property::LineRoundLimit)}});
} // namespace

optional<Error> LineLayer::setProperty(const std::string& name, const Convertible& value) {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        if (name == "visibility") return setVisibility(value);
        return Error{"layer doesn't support this property"};
    }

    auto property = static_cast<Property>(it->second);

    if (property == Property::LineBlur || property == Property::LineGapWidth || property == Property::LineOffset ||
        property == Property::LineOpacity || property == Property::LineWidth) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::LineBlur) {
            setLineBlur(*typedValue);
            return nullopt;
        }

        if (property == Property::LineGapWidth) {
            setLineGapWidth(*typedValue);
            return nullopt;
        }

        if (property == Property::LineOffset) {
            setLineOffset(*typedValue);
            return nullopt;
        }

        if (property == Property::LineOpacity) {
            setLineOpacity(*typedValue);
            return nullopt;
        }

        if (property == Property::LineWidth) {
            setLineWidth(*typedValue);
            return nullopt;
        }
    }
    if (property == Property::LineColor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setLineColor(*typedValue);
        return nullopt;
    }
    if (property == Property::LineDasharray) {
        Error error;
        const auto& typedValue = convert<PropertyValue<std::vector<float>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setLineDasharray(*typedValue);
        return nullopt;
    }
    if (property == Property::LineGradient) {
        Error error;
        const auto& typedValue = convert<ColorRampPropertyValue>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setLineGradient(*typedValue);
        return nullopt;
    }
    if (property == Property::LinePattern) {
        Error error;
        const auto& typedValue = convert<PropertyValue<expression::Image>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setLinePattern(*typedValue);
        return nullopt;
    }
    if (property == Property::LineTranslate) {
        Error error;
        const auto& typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setLineTranslate(*typedValue);
        return nullopt;
    }
    if (property == Property::LineTranslateAnchor) {
        Error error;
        const auto& typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setLineTranslateAnchor(*typedValue);
        return nullopt;
    }
    if (property == Property::LineCap) {
        Error error;
        const auto& typedValue = convert<PropertyValue<LineCapType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        setLineCap(*typedValue);
        return nullopt;
    }
    if (property == Property::LineJoin) {
        Error error;
        const auto& typedValue = convert<PropertyValue<LineJoinType>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }

        setLineJoin(*typedValue);
        return nullopt;
    }
    if (property == Property::LineMiterLimit || property == Property::LineRoundLimit) {
        Error error;
        const auto& typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }

        if (property == Property::LineMiterLimit) {
            setLineMiterLimit(*typedValue);
            return nullopt;
        }

        if (property == Property::LineRoundLimit) {
            setLineRoundLimit(*typedValue);
            return nullopt;
        }
    }

    Error error;
    optional<TransitionOptions> transition = convert<TransitionOptions>(value, error);
    if (!transition) {
        return error;
    }

    if (property == Property::LineBlurTransition) {
        setLineBlurTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineColorTransition) {
        setLineColorTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineDasharrayTransition) {
        setLineDasharrayTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineGapWidthTransition) {
        setLineGapWidthTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineGradientTransition) {
        setLineGradientTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineOffsetTransition) {
        setLineOffsetTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineOpacityTransition) {
        setLineOpacityTransition(*transition);
        return nullopt;
    }

    if (property == Property::LinePatternTransition) {
        setLinePatternTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineTranslateTransition) {
        setLineTranslateTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineTranslateAnchorTransition) {
        setLineTranslateAnchorTransition(*transition);
        return nullopt;
    }

    if (property == Property::LineWidthTransition) {
        setLineWidthTransition(*transition);
        return nullopt;
    }

    return Error{"layer doesn't support this property"};
}

StyleProperty LineLayer::getProperty(const std::string& name) const {
    const auto it = layerProperties.find(name.c_str());
    if (it == layerProperties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::LineBlur:
            return makeStyleProperty(getLineBlur());
        case Property::LineColor:
            return makeStyleProperty(getLineColor());
        case Property::LineDasharray:
            return makeStyleProperty(getLineDasharray());
        case Property::LineGapWidth:
            return makeStyleProperty(getLineGapWidth());
        case Property::LineGradient:
            return makeStyleProperty(getLineGradient());
        case Property::LineOffset:
            return makeStyleProperty(getLineOffset());
        case Property::LineOpacity:
            return makeStyleProperty(getLineOpacity());
        case Property::LinePattern:
            return makeStyleProperty(getLinePattern());
        case Property::LineTranslate:
            return makeStyleProperty(getLineTranslate());
        case Property::LineTranslateAnchor:
            return makeStyleProperty(getLineTranslateAnchor());
        case Property::LineWidth:
            return makeStyleProperty(getLineWidth());
        case Property::LineBlurTransition:
            return makeStyleProperty(getLineBlurTransition());
        case Property::LineColorTransition:
            return makeStyleProperty(getLineColorTransition());
        case Property::LineDasharrayTransition:
            return makeStyleProperty(getLineDasharrayTransition());
        case Property::LineGapWidthTransition:
            return makeStyleProperty(getLineGapWidthTransition());
        case Property::LineGradientTransition:
            return makeStyleProperty(getLineGradientTransition());
        case Property::LineOffsetTransition:
            return makeStyleProperty(getLineOffsetTransition());
        case Property::LineOpacityTransition:
            return makeStyleProperty(getLineOpacityTransition());
        case Property::LinePatternTransition:
            return makeStyleProperty(getLinePatternTransition());
        case Property::LineTranslateTransition:
            return makeStyleProperty(getLineTranslateTransition());
        case Property::LineTranslateAnchorTransition:
            return makeStyleProperty(getLineTranslateAnchorTransition());
        case Property::LineWidthTransition:
            return makeStyleProperty(getLineWidthTransition());
        case Property::LineCap:
            return makeStyleProperty(getLineCap());
        case Property::LineJoin:
            return makeStyleProperty(getLineJoin());
        case Property::LineMiterLimit:
            return makeStyleProperty(getLineMiterLimit());
        case Property::LineRoundLimit:
            return makeStyleProperty(getLineRoundLimit());
    }
    return {};
}

Mutable<Layer::Impl> LineLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
