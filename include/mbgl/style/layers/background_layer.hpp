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

class BackgroundLayer : public Layer {
public:
    BackgroundLayer(const std::string& layerID);
    ~BackgroundLayer() final;

    // Visibility
    void setVisibility(VisibilityType) final;

    // Zoom range
    void setMinZoom(float) final;
    void setMaxZoom(float) final;

    // Paint properties

    static PropertyValue<Color> getDefaultBackgroundColor();
    PropertyValue<Color> getBackgroundColor() const;
    void setBackgroundColor(PropertyValue<Color>);
    void setBackgroundColorTransition(const TransitionOptions&);
    TransitionOptions getBackgroundColorTransition() const;

    static PropertyValue<std::string> getDefaultBackgroundPattern();
    PropertyValue<std::string> getBackgroundPattern() const;
    void setBackgroundPattern(PropertyValue<std::string>);
    void setBackgroundPatternTransition(const TransitionOptions&);
    TransitionOptions getBackgroundPatternTransition() const;

    static PropertyValue<float> getDefaultBackgroundOpacity();
    PropertyValue<float> getBackgroundOpacity() const;
    void setBackgroundOpacity(PropertyValue<float>);
    void setBackgroundOpacityTransition(const TransitionOptions&);
    TransitionOptions getBackgroundOpacityTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    BackgroundLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;
};

template <>
inline bool Layer::is<BackgroundLayer>() const {
    return getType() == LayerType::Background;
}

} // namespace style
} // namespace mbgl
