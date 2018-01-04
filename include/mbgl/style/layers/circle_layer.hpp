// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class CircleLayer : public Layer {
public:
    CircleLayer(const std::string& layerID, const std::string& sourceID);
    ~CircleLayer() final;

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

    // Paint properties

    static DataDrivenPropertyValue<float> getDefaultCircleRadius();
    DataDrivenPropertyValue<float> getCircleRadius() const;
    void setCircleRadius(DataDrivenPropertyValue<float>);
    void setCircleRadiusTransition(const TransitionOptions&);
    TransitionOptions getCircleRadiusTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultCircleColor();
    DataDrivenPropertyValue<Color> getCircleColor() const;
    void setCircleColor(DataDrivenPropertyValue<Color>);
    void setCircleColorTransition(const TransitionOptions&);
    TransitionOptions getCircleColorTransition() const;

    static DataDrivenPropertyValue<float> getDefaultCircleBlur();
    DataDrivenPropertyValue<float> getCircleBlur() const;
    void setCircleBlur(DataDrivenPropertyValue<float>);
    void setCircleBlurTransition(const TransitionOptions&);
    TransitionOptions getCircleBlurTransition() const;

    static DataDrivenPropertyValue<float> getDefaultCircleOpacity();
    DataDrivenPropertyValue<float> getCircleOpacity() const;
    void setCircleOpacity(DataDrivenPropertyValue<float>);
    void setCircleOpacityTransition(const TransitionOptions&);
    TransitionOptions getCircleOpacityTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultCircleTranslate();
    PropertyValue<std::array<float, 2>> getCircleTranslate() const;
    void setCircleTranslate(PropertyValue<std::array<float, 2>>);
    void setCircleTranslateTransition(const TransitionOptions&);
    TransitionOptions getCircleTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultCircleTranslateAnchor();
    PropertyValue<TranslateAnchorType> getCircleTranslateAnchor() const;
    void setCircleTranslateAnchor(PropertyValue<TranslateAnchorType>);
    void setCircleTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getCircleTranslateAnchorTransition() const;

    static PropertyValue<CirclePitchScaleType> getDefaultCirclePitchScale();
    PropertyValue<CirclePitchScaleType> getCirclePitchScale() const;
    void setCirclePitchScale(PropertyValue<CirclePitchScaleType>);
    void setCirclePitchScaleTransition(const TransitionOptions&);
    TransitionOptions getCirclePitchScaleTransition() const;

    static PropertyValue<AlignmentType> getDefaultCirclePitchAlignment();
    PropertyValue<AlignmentType> getCirclePitchAlignment() const;
    void setCirclePitchAlignment(PropertyValue<AlignmentType>);
    void setCirclePitchAlignmentTransition(const TransitionOptions&);
    TransitionOptions getCirclePitchAlignmentTransition() const;

    static DataDrivenPropertyValue<float> getDefaultCircleStrokeWidth();
    DataDrivenPropertyValue<float> getCircleStrokeWidth() const;
    void setCircleStrokeWidth(DataDrivenPropertyValue<float>);
    void setCircleStrokeWidthTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeWidthTransition() const;

    static DataDrivenPropertyValue<Color> getDefaultCircleStrokeColor();
    DataDrivenPropertyValue<Color> getCircleStrokeColor() const;
    void setCircleStrokeColor(DataDrivenPropertyValue<Color>);
    void setCircleStrokeColorTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeColorTransition() const;

    static DataDrivenPropertyValue<float> getDefaultCircleStrokeOpacity();
    DataDrivenPropertyValue<float> getCircleStrokeOpacity() const;
    void setCircleStrokeOpacity(DataDrivenPropertyValue<float>);
    void setCircleStrokeOpacityTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeOpacityTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    CircleLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;
};

template <>
inline bool Layer::is<CircleLayer>() const {
    return getType() == LayerType::Circle;
}

} // namespace style
} // namespace mbgl
