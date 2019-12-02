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

class BackgroundLayer : public Layer {
public:
    BackgroundLayer(const std::string& layerID);
    ~BackgroundLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Paint properties

    static PropertyValue<Color> getDefaultBackgroundColor();
    const PropertyValue<Color>& getBackgroundColor() const;
    void setBackgroundColor(const PropertyValue<Color>&);
    void setBackgroundColorTransition(const TransitionOptions&);
    TransitionOptions getBackgroundColorTransition() const;

    static PropertyValue<float> getDefaultBackgroundOpacity();
    const PropertyValue<float>& getBackgroundOpacity() const;
    void setBackgroundOpacity(const PropertyValue<float>&);
    void setBackgroundOpacityTransition(const TransitionOptions&);
    TransitionOptions getBackgroundOpacityTransition() const;

    static PropertyValue<expression::Image> getDefaultBackgroundPattern();
    const PropertyValue<expression::Image>& getBackgroundPattern() const;
    void setBackgroundPattern(const PropertyValue<expression::Image>&);
    void setBackgroundPatternTransition(const TransitionOptions&);
    TransitionOptions getBackgroundPatternTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    BackgroundLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
