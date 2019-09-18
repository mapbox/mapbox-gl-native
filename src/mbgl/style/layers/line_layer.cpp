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
    return { 0 };
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
    return { Color::black() };
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
    return { {  } };
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
    return { 0 };
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
    return { {} };
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
    return { 0 };
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
    return { 1 };
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

PropertyValue<std::string> LineLayer::getDefaultLinePattern() {
    return { "" };
}

const PropertyValue<std::string>& LineLayer::getLinePattern() const {
    return impl().paint.template get<LinePattern>().value;
}

void LineLayer::setLinePattern(const PropertyValue<std::string>& value) {
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
    return { {{ 0, 0 }} };
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
    return { TranslateAnchorType::Map };
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
    return { 1 };
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

optional<Error> LineLayer::setPaintProperty(const std::string& name, const Convertible& value) {
    enum class Property {
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
    };

    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "line-blur", mbgl::underlying_type(Property::LineBlur) },
        { "line-color", mbgl::underlying_type(Property::LineColor) },
        { "line-dasharray", mbgl::underlying_type(Property::LineDasharray) },
        { "line-gap-width", mbgl::underlying_type(Property::LineGapWidth) },
        { "line-gradient", mbgl::underlying_type(Property::LineGradient) },
        { "line-offset", mbgl::underlying_type(Property::LineOffset) },
        { "line-opacity", mbgl::underlying_type(Property::LineOpacity) },
        { "line-pattern", mbgl::underlying_type(Property::LinePattern) },
        { "line-translate", mbgl::underlying_type(Property::LineTranslate) },
        { "line-translate-anchor", mbgl::underlying_type(Property::LineTranslateAnchor) },
        { "line-width", mbgl::underlying_type(Property::LineWidth) },
        { "line-blur-transition", mbgl::underlying_type(Property::LineBlurTransition) },
        { "line-color-transition", mbgl::underlying_type(Property::LineColorTransition) },
        { "line-dasharray-transition", mbgl::underlying_type(Property::LineDasharrayTransition) },
        { "line-gap-width-transition", mbgl::underlying_type(Property::LineGapWidthTransition) },
        { "line-gradient-transition", mbgl::underlying_type(Property::LineGradientTransition) },
        { "line-offset-transition", mbgl::underlying_type(Property::LineOffsetTransition) },
        { "line-opacity-transition", mbgl::underlying_type(Property::LineOpacityTransition) },
        { "line-pattern-transition", mbgl::underlying_type(Property::LinePatternTransition) },
        { "line-translate-transition", mbgl::underlying_type(Property::LineTranslateTransition) },
        { "line-translate-anchor-transition", mbgl::underlying_type(Property::LineTranslateAnchorTransition) },
        { "line-width-transition", mbgl::underlying_type(Property::LineWidthTransition) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);

        
    if (property == Property::LineBlur || property == Property::LineGapWidth || property == Property::LineOffset || property == Property::LineOpacity || property == Property::LineWidth) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, true, false);
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
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setLineColor(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::LineDasharray) {
        Error error;
        optional<PropertyValue<std::vector<float>>> typedValue = convert<PropertyValue<std::vector<float>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setLineDasharray(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::LineGradient) {
        Error error;
        optional<ColorRampPropertyValue> typedValue = convert<ColorRampPropertyValue>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setLineGradient(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::LinePattern) {
        Error error;
        optional<PropertyValue<std::string>> typedValue = convert<PropertyValue<std::string>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setLinePattern(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::LineTranslate) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setLineTranslate(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::LineTranslateAnchor) {
        Error error;
        optional<PropertyValue<TranslateAnchorType>> typedValue = convert<PropertyValue<TranslateAnchorType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setLineTranslateAnchor(*typedValue);
        return nullopt;
        
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
    

    return Error { "layer doesn't support this property" };
}

optional<Error> LineLayer::setLayoutProperty(const std::string& name, const Convertible& value) {
    if (name == "visibility") {
        return Layer::setVisibility(value);
    }
    enum class Property {
        LineCap,
        LineJoin,
        LineMiterLimit,
        LineRoundLimit,
    };
    MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>({
        { "line-cap", mbgl::underlying_type(Property::LineCap) },
        { "line-join", mbgl::underlying_type(Property::LineJoin) },
        { "line-miter-limit", mbgl::underlying_type(Property::LineMiterLimit) },
        { "line-round-limit", mbgl::underlying_type(Property::LineRoundLimit) }
    });

    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "layer doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);

        
    if (property == Property::LineCap) {
        Error error;
        optional<PropertyValue<LineCapType>> typedValue = convert<PropertyValue<LineCapType>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setLineCap(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::LineJoin) {
        Error error;
        optional<PropertyValue<LineJoinType>> typedValue = convert<PropertyValue<LineJoinType>>(value, error, true, false);
        if (!typedValue) {
            return error;
        }
        
        setLineJoin(*typedValue);
        return nullopt;
        
    }
    
    if (property == Property::LineMiterLimit || property == Property::LineRoundLimit) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
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
    

    return Error { "layer doesn't support this property" };
}

Mutable<Layer::Impl> LineLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

} // namespace style
} // namespace mbgl
