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

class FillExtrusionLayer : public Layer {
public:
    FillExtrusionLayer(const std::string& layerID, const std::string& sourceID);
    ~FillExtrusionLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Paint properties

    static PropertyValue<float> getDefaultFillExtrusionBase();
    const PropertyValue<float>& getFillExtrusionBase() const;
    void setFillExtrusionBase(const PropertyValue<float>&);
    void setFillExtrusionBaseTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionBaseTransition() const;

    static PropertyValue<Color> getDefaultFillExtrusionColor();
    const PropertyValue<Color>& getFillExtrusionColor() const;
    void setFillExtrusionColor(const PropertyValue<Color>&);
    void setFillExtrusionColorTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionColorTransition() const;

    static PropertyValue<float> getDefaultFillExtrusionHeight();
    const PropertyValue<float>& getFillExtrusionHeight() const;
    void setFillExtrusionHeight(const PropertyValue<float>&);
    void setFillExtrusionHeightTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionHeightTransition() const;

    static PropertyValue<float> getDefaultFillExtrusionOpacity();
    const PropertyValue<float>& getFillExtrusionOpacity() const;
    void setFillExtrusionOpacity(const PropertyValue<float>&);
    void setFillExtrusionOpacityTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionOpacityTransition() const;

    static PropertyValue<expression::Image> getDefaultFillExtrusionPattern();
    const PropertyValue<expression::Image>& getFillExtrusionPattern() const;
    void setFillExtrusionPattern(const PropertyValue<expression::Image>&);
    void setFillExtrusionPatternTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionPatternTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultFillExtrusionTranslate();
    const PropertyValue<std::array<float, 2>>& getFillExtrusionTranslate() const;
    void setFillExtrusionTranslate(const PropertyValue<std::array<float, 2>>&);
    void setFillExtrusionTranslateTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultFillExtrusionTranslateAnchor();
    const PropertyValue<TranslateAnchorType>& getFillExtrusionTranslateAnchor() const;
    void setFillExtrusionTranslateAnchor(const PropertyValue<TranslateAnchorType>&);
    void setFillExtrusionTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionTranslateAnchorTransition() const;

    static PropertyValue<bool> getDefaultFillExtrusionVerticalGradient();
    const PropertyValue<bool>& getFillExtrusionVerticalGradient() const;
    void setFillExtrusionVerticalGradient(const PropertyValue<bool>&);
    void setFillExtrusionVerticalGradientTransition(const TransitionOptions&);
    TransitionOptions getFillExtrusionVerticalGradientTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    FillExtrusionLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
