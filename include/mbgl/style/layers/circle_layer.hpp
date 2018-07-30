// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

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

    // Dynamic properties
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Paint properties

    static PropertyValue<float> getDefaultCircleRadius();
    PropertyValue<float> getCircleRadius() const;
    void setCircleRadius(PropertyValue<float>);
    void setCircleRadiusTransition(const TransitionOptions&);
    TransitionOptions getCircleRadiusTransition() const;

    static PropertyValue<Color> getDefaultCircleColor();
    PropertyValue<Color> getCircleColor() const;
    void setCircleColor(PropertyValue<Color>);
    void setCircleColorTransition(const TransitionOptions&);
    TransitionOptions getCircleColorTransition() const;

    static PropertyValue<float> getDefaultCircleBlur();
    PropertyValue<float> getCircleBlur() const;
    void setCircleBlur(PropertyValue<float>);
    void setCircleBlurTransition(const TransitionOptions&);
    TransitionOptions getCircleBlurTransition() const;

    static PropertyValue<float> getDefaultCircleOpacity();
    PropertyValue<float> getCircleOpacity() const;
    void setCircleOpacity(PropertyValue<float>);
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

    static PropertyValue<float> getDefaultCircleStrokeWidth();
    PropertyValue<float> getCircleStrokeWidth() const;
    void setCircleStrokeWidth(PropertyValue<float>);
    void setCircleStrokeWidthTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeWidthTransition() const;

    static PropertyValue<Color> getDefaultCircleStrokeColor();
    PropertyValue<Color> getCircleStrokeColor() const;
    void setCircleStrokeColor(PropertyValue<Color>);
    void setCircleStrokeColorTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeColorTransition() const;

    static PropertyValue<float> getDefaultCircleStrokeOpacity();
    PropertyValue<float> getCircleStrokeOpacity() const;
    void setCircleStrokeOpacity(PropertyValue<float>);
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
