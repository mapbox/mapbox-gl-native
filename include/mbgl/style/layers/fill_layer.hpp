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

class FillLayer : public Layer {
public:
    FillLayer(const std::string& layerID, const std::string& sourceID);
    ~FillLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Paint properties

    static PropertyValue<bool> getDefaultFillAntialias();
    const PropertyValue<bool>& getFillAntialias() const;
    void setFillAntialias(const PropertyValue<bool>&);
    void setFillAntialiasTransition(const TransitionOptions&);
    TransitionOptions getFillAntialiasTransition() const;

    static PropertyValue<Color> getDefaultFillColor();
    const PropertyValue<Color>& getFillColor() const;
    void setFillColor(const PropertyValue<Color>&);
    void setFillColorTransition(const TransitionOptions&);
    TransitionOptions getFillColorTransition() const;

    static PropertyValue<float> getDefaultFillOpacity();
    const PropertyValue<float>& getFillOpacity() const;
    void setFillOpacity(const PropertyValue<float>&);
    void setFillOpacityTransition(const TransitionOptions&);
    TransitionOptions getFillOpacityTransition() const;

    static PropertyValue<Color> getDefaultFillOutlineColor();
    const PropertyValue<Color>& getFillOutlineColor() const;
    void setFillOutlineColor(const PropertyValue<Color>&);
    void setFillOutlineColorTransition(const TransitionOptions&);
    TransitionOptions getFillOutlineColorTransition() const;

    static PropertyValue<expression::Image> getDefaultFillPattern();
    const PropertyValue<expression::Image>& getFillPattern() const;
    void setFillPattern(const PropertyValue<expression::Image>&);
    void setFillPatternTransition(const TransitionOptions&);
    TransitionOptions getFillPatternTransition() const;

    static PropertyValue<std::array<float, 2>> getDefaultFillTranslate();
    const PropertyValue<std::array<float, 2>>& getFillTranslate() const;
    void setFillTranslate(const PropertyValue<std::array<float, 2>>&);
    void setFillTranslateTransition(const TransitionOptions&);
    TransitionOptions getFillTranslateTransition() const;

    static PropertyValue<TranslateAnchorType> getDefaultFillTranslateAnchor();
    const PropertyValue<TranslateAnchorType>& getFillTranslateAnchor() const;
    void setFillTranslateAnchor(const PropertyValue<TranslateAnchorType>&);
    void setFillTranslateAnchorTransition(const TransitionOptions&);
    TransitionOptions getFillTranslateAnchorTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    FillLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
