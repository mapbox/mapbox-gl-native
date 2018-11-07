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
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Layout properties

    static PropertyValue<LineCapType> getDefaultLineCap();
    PropertyValue<LineCapType> getLineCap() const;
    void setLineCap(PropertyValue<LineCapType>);

    static PropertyValue<LineJoinType> getDefaultLineJoin();
    PropertyValue<LineJoinType> getLineJoin() const;
    void setLineJoin(PropertyValue<LineJoinType>);

    static PropertyValue<float> getDefaultLineMiterLimit();
    PropertyValue<float> getLineMiterLimit() const;
    void setLineMiterLimit(PropertyValue<float>);

    static PropertyValue<float> getDefaultLineRoundLimit();
    PropertyValue<float> getLineRoundLimit() const;
    void setLineRoundLimit(PropertyValue<float>);

    // Paint properties

    static PropertyValue<float> getDefaultLineOpacity();
    PropertyValue<float> getLineOpacity() const;
    void setLineOpacity(PropertyValue<float>);
    void setLineOpacityTransition(const TransitionOptions&);
    TransitionOptions getLineOpacityTransition() const;

    static PropertyValue<Color> getDefaultLineColor();
    PropertyValue<Color> getLineColor() const;
    void setLineColor(PropertyValue<Color>);
    void setLineColorTransition(const TransitionOptions&);
    TransitionOptions getLineColorTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultLineTranslate();
    PropertyValue<std::array<float, 2>> getLineTranslate() const;
    void setLineTranslate(PropertyValue<std::array<float, 2>>);
    void setLineTranslateTransition(const TransitionOptions&);
    TransitionOptions getLineTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultLineTranslateAnchor();
    PropertyValue<TranslateAnchorType> getLineTranslateAnchor() const;
    void setLineTranslateAnchor(PropertyValue<TranslateAnchorType>);
    void setLineTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getLineTranslateAnchorTransition() const;

    static PropertyValue<float> getDefaultLineWidth();
    PropertyValue<float> getLineWidth() const;
    void setLineWidth(PropertyValue<float>);
    void setLineWidthTransition(const TransitionOptions&);
    TransitionOptions getLineWidthTransition() const;

    static PropertyValue<float> getDefaultLineGapWidth();
    PropertyValue<float> getLineGapWidth() const;
    void setLineGapWidth(PropertyValue<float>);
    void setLineGapWidthTransition(const TransitionOptions&);
    TransitionOptions getLineGapWidthTransition() const;

    static PropertyValue<float> getDefaultLineOffset();
    PropertyValue<float> getLineOffset() const;
    void setLineOffset(PropertyValue<float>);
    void setLineOffsetTransition(const TransitionOptions&);
    TransitionOptions getLineOffsetTransition() const;

    static PropertyValue<float> getDefaultLineBlur();
    PropertyValue<float> getLineBlur() const;
    void setLineBlur(PropertyValue<float>);
    void setLineBlurTransition(const TransitionOptions&);
    TransitionOptions getLineBlurTransition() const;

    static PropertyValue<std::vector<float>> getDefaultLineDasharray();
    PropertyValue<std::vector<float>> getLineDasharray() const;
    void setLineDasharray(PropertyValue<std::vector<float>>);
    void setLineDasharrayTransition(const TransitionOptions&);
    TransitionOptions getLineDasharrayTransition() const;

    static PropertyValue<std::string> getDefaultLinePattern();
    PropertyValue<std::string> getLinePattern() const;
    void setLinePattern(PropertyValue<std::string>);
    void setLinePatternTransition(const TransitionOptions&);
    TransitionOptions getLinePatternTransition() const;

    static ColorRampPropertyValue getDefaultLineGradient();
    ColorRampPropertyValue getLineGradient() const;
    void setLineGradient(ColorRampPropertyValue);
    void setLineGradientTransition(const TransitionOptions&);
    TransitionOptions getLineGradientTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    LineLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

class LineLayerFactory : public LayerFactory {
public:
    LineLayerFactory();
    // LayerFactory overrides.
    ~LineLayerFactory() override;
    bool supportsType(const std::string& type) const final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const conversion::Convertible& value) final;

    static LineLayerFactory* get();

private:
    static LineLayerFactory* instance;
};

} // namespace style
} // namespace mbgl
