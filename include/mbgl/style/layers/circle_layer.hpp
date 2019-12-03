// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class CircleLayer : public Layer {
public:
    CircleLayer(const std::string& layerID, const std::string& sourceID);
    ~CircleLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Paint properties

    static PropertyValue<float> getDefaultCircleBlur();
    const PropertyValue<float>& getCircleBlur() const;
    void setCircleBlur(const PropertyValue<float>&);
    void setCircleBlurTransition(const TransitionOptions&);
    TransitionOptions getCircleBlurTransition() const;

    static PropertyValue<Color> getDefaultCircleColor();
    const PropertyValue<Color>& getCircleColor() const;
    void setCircleColor(const PropertyValue<Color>&);
    void setCircleColorTransition(const TransitionOptions&);
    TransitionOptions getCircleColorTransition() const;

    static PropertyValue<float> getDefaultCircleOpacity();
    const PropertyValue<float>& getCircleOpacity() const;
    void setCircleOpacity(const PropertyValue<float>&);
    void setCircleOpacityTransition(const TransitionOptions&);
    TransitionOptions getCircleOpacityTransition() const;

    static PropertyValue<AlignmentType> getDefaultCirclePitchAlignment();
    const PropertyValue<AlignmentType>& getCirclePitchAlignment() const;
    void setCirclePitchAlignment(const PropertyValue<AlignmentType>&);
    void setCirclePitchAlignmentTransition(const TransitionOptions&);
    TransitionOptions getCirclePitchAlignmentTransition() const;

    static PropertyValue<CirclePitchScaleType> getDefaultCirclePitchScale();
    const PropertyValue<CirclePitchScaleType>& getCirclePitchScale() const;
    void setCirclePitchScale(const PropertyValue<CirclePitchScaleType>&);
    void setCirclePitchScaleTransition(const TransitionOptions&);
    TransitionOptions getCirclePitchScaleTransition() const;

    static PropertyValue<float> getDefaultCircleRadius();
    const PropertyValue<float>& getCircleRadius() const;
    void setCircleRadius(const PropertyValue<float>&);
    void setCircleRadiusTransition(const TransitionOptions&);
    TransitionOptions getCircleRadiusTransition() const;

    static PropertyValue<Color> getDefaultCircleStrokeColor();
    const PropertyValue<Color>& getCircleStrokeColor() const;
    void setCircleStrokeColor(const PropertyValue<Color>&);
    void setCircleStrokeColorTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeColorTransition() const;

    static PropertyValue<float> getDefaultCircleStrokeOpacity();
    const PropertyValue<float>& getCircleStrokeOpacity() const;
    void setCircleStrokeOpacity(const PropertyValue<float>&);
    void setCircleStrokeOpacityTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeOpacityTransition() const;

    static PropertyValue<float> getDefaultCircleStrokeWidth();
    const PropertyValue<float>& getCircleStrokeWidth() const;
    void setCircleStrokeWidth(const PropertyValue<float>&);
    void setCircleStrokeWidthTransition(const TransitionOptions&);
    TransitionOptions getCircleStrokeWidthTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultCircleTranslate();
    const PropertyValue<std::array<float, 2>>& getCircleTranslate() const;
    void setCircleTranslate(const PropertyValue<std::array<float, 2>>&);
    void setCircleTranslateTransition(const TransitionOptions&);
    TransitionOptions getCircleTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultCircleTranslateAnchor();
    const PropertyValue<TranslateAnchorType>& getCircleTranslateAnchor() const;
    void setCircleTranslateAnchor(const PropertyValue<TranslateAnchorType>&);
    void setCircleTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getCircleTranslateAnchorTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    CircleLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
