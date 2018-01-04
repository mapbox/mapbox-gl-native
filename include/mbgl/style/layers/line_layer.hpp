// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/color.hpp>

#include <vector>

namespace mbgl {
namespace style {

class TransitionOptions;

class LineLayer : public Layer {
public:
    LineLayer(const std::string& layerID, const std::string& sourceID);
    ~LineLayer() final;

    // Source
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;
    void setSourceLayer(const std::string& sourceLayer);

    void setFilter(const Filter&);
    const Filter& getFilter() const;

    // Visibility
    void setVisibility(VisibilityType) final;

    // Zoom range
    void setMinZoom(float) final;
    void setMaxZoom(float) final;

    // Layout properties

    static PropertyValue<LineCapType> getDefaultLineCap();
    PropertyValue<LineCapType> getLineCap() const;
    void setLineCap(PropertyValue<LineCapType>);

    static DataDrivenPropertyValue<LineJoinType> getDefaultLineJoin();
    DataDrivenPropertyValue<LineJoinType> getLineJoin() const;
    void setLineJoin(DataDrivenPropertyValue<LineJoinType>);

    static PropertyValue<float> getDefaultLineMiterLimit();
    PropertyValue<float> getLineMiterLimit() const;
    void setLineMiterLimit(PropertyValue<float>);

    static PropertyValue<float> getDefaultLineRoundLimit();
    PropertyValue<float> getLineRoundLimit() const;
    void setLineRoundLimit(PropertyValue<float>);

    // Paint properties

    static DataDrivenPropertyValue<float> getDefaultLineOpacity();
    DataDrivenPropertyValue<float> getLineOpacity() const;
    void setLineOpacity(DataDrivenPropertyValue<float>);
    void setLineOpacityTransition(const TransitionOptions&);
    TransitionOptions getLineOpacityTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultLineColor();
    DataDrivenPropertyValue<Color> getLineColor() const;
    void setLineColor(DataDrivenPropertyValue<Color>);
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

    static DataDrivenPropertyValue<float> getDefaultLineWidth();
    DataDrivenPropertyValue<float> getLineWidth() const;
    void setLineWidth(DataDrivenPropertyValue<float>);
    void setLineWidthTransition(const TransitionOptions&);
    TransitionOptions getLineWidthTransition() const;

    static DataDrivenPropertyValue<float> getDefaultLineGapWidth();
    DataDrivenPropertyValue<float> getLineGapWidth() const;
    void setLineGapWidth(DataDrivenPropertyValue<float>);
    void setLineGapWidthTransition(const TransitionOptions&);
    TransitionOptions getLineGapWidthTransition() const;

    static DataDrivenPropertyValue<float> getDefaultLineOffset();
    DataDrivenPropertyValue<float> getLineOffset() const;
    void setLineOffset(DataDrivenPropertyValue<float>);
    void setLineOffsetTransition(const TransitionOptions&);
    TransitionOptions getLineOffsetTransition() const;

    static DataDrivenPropertyValue<float> getDefaultLineBlur();
    DataDrivenPropertyValue<float> getLineBlur() const;
    void setLineBlur(DataDrivenPropertyValue<float>);
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

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    LineLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;
};

template <>
inline bool Layer::is<LineLayer>() const {
    return getType() == LayerType::Line;
}

} // namespace style
} // namespace mbgl
