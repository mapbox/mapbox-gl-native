// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/color_ramp_property_value.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/color.hpp>

#include <vector>

namespace mbgl {
namespace style {

class TransitionOptions;

class LineLayer : public Layer {
public:
    LineLayer(const std::string& layerID, const std::string& sourceID);
    ~LineLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Layout properties

    static PropertyValue<LineCapType> getDefaultLineCap();
    const PropertyValue<LineCapType>& getLineCap() const;
    void setLineCap(const PropertyValue<LineCapType>&);

    static PropertyValue<LineJoinType> getDefaultLineJoin();
    const PropertyValue<LineJoinType>& getLineJoin() const;
    void setLineJoin(const PropertyValue<LineJoinType>&);

    static PropertyValue<float> getDefaultLineMiterLimit();
    const PropertyValue<float>& getLineMiterLimit() const;
    void setLineMiterLimit(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultLineRoundLimit();
    const PropertyValue<float>& getLineRoundLimit() const;
    void setLineRoundLimit(const PropertyValue<float>&);

    // Paint properties

    static PropertyValue<float> getDefaultLineBlur();
    const PropertyValue<float>& getLineBlur() const;
    void setLineBlur(const PropertyValue<float>&);
    void setLineBlurTransition(const TransitionOptions&);
    TransitionOptions getLineBlurTransition() const;

    static PropertyValue<Color> getDefaultLineColor();
    const PropertyValue<Color>& getLineColor() const;
    void setLineColor(const PropertyValue<Color>&);
    void setLineColorTransition(const TransitionOptions&);
    TransitionOptions getLineColorTransition() const;

    static PropertyValue<std::vector<float>> getDefaultLineDasharray();
    const PropertyValue<std::vector<float>>& getLineDasharray() const;
    void setLineDasharray(const PropertyValue<std::vector<float>>&);
    void setLineDasharrayTransition(const TransitionOptions&);
    TransitionOptions getLineDasharrayTransition() const;

    static PropertyValue<float> getDefaultLineGapWidth();
    const PropertyValue<float>& getLineGapWidth() const;
    void setLineGapWidth(const PropertyValue<float>&);
    void setLineGapWidthTransition(const TransitionOptions&);
    TransitionOptions getLineGapWidthTransition() const;

    static ColorRampPropertyValue getDefaultLineGradient();
    const ColorRampPropertyValue& getLineGradient() const;
    void setLineGradient(const ColorRampPropertyValue&);
    void setLineGradientTransition(const TransitionOptions&);
    TransitionOptions getLineGradientTransition() const;

    static PropertyValue<float> getDefaultLineOffset();
    const PropertyValue<float>& getLineOffset() const;
    void setLineOffset(const PropertyValue<float>&);
    void setLineOffsetTransition(const TransitionOptions&);
    TransitionOptions getLineOffsetTransition() const;

    static PropertyValue<float> getDefaultLineOpacity();
    const PropertyValue<float>& getLineOpacity() const;
    void setLineOpacity(const PropertyValue<float>&);
    void setLineOpacityTransition(const TransitionOptions&);
    TransitionOptions getLineOpacityTransition() const;

    static PropertyValue<expression::Image> getDefaultLinePattern();
    const PropertyValue<expression::Image>& getLinePattern() const;
    void setLinePattern(const PropertyValue<expression::Image>&);
    void setLinePatternTransition(const TransitionOptions&);
    TransitionOptions getLinePatternTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultLineTranslate();
    const PropertyValue<std::array<float, 2>>& getLineTranslate() const;
    void setLineTranslate(const PropertyValue<std::array<float, 2>>&);
    void setLineTranslateTransition(const TransitionOptions&);
    TransitionOptions getLineTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultLineTranslateAnchor();
    const PropertyValue<TranslateAnchorType>& getLineTranslateAnchor() const;
    void setLineTranslateAnchor(const PropertyValue<TranslateAnchorType>&);
    void setLineTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getLineTranslateAnchorTransition() const;

    static PropertyValue<float> getDefaultLineWidth();
    const PropertyValue<float>& getLineWidth() const;
    void setLineWidth(const PropertyValue<float>&);
    void setLineWidthTransition(const TransitionOptions&);
    TransitionOptions getLineWidthTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    LineLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
