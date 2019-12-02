// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/color_ramp_property_value.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class HeatmapLayer : public Layer {
public:
    HeatmapLayer(const std::string& layerID, const std::string& sourceID);
    ~HeatmapLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Paint properties

    static ColorRampPropertyValue getDefaultHeatmapColor();
    const ColorRampPropertyValue& getHeatmapColor() const;
    void setHeatmapColor(const ColorRampPropertyValue&);
    void setHeatmapColorTransition(const TransitionOptions&);
    TransitionOptions getHeatmapColorTransition() const;

    static PropertyValue<float> getDefaultHeatmapIntensity();
    const PropertyValue<float>& getHeatmapIntensity() const;
    void setHeatmapIntensity(const PropertyValue<float>&);
    void setHeatmapIntensityTransition(const TransitionOptions&);
    TransitionOptions getHeatmapIntensityTransition() const;

    static PropertyValue<float> getDefaultHeatmapOpacity();
    const PropertyValue<float>& getHeatmapOpacity() const;
    void setHeatmapOpacity(const PropertyValue<float>&);
    void setHeatmapOpacityTransition(const TransitionOptions&);
    TransitionOptions getHeatmapOpacityTransition() const;

    static PropertyValue<float> getDefaultHeatmapRadius();
    const PropertyValue<float>& getHeatmapRadius() const;
    void setHeatmapRadius(const PropertyValue<float>&);
    void setHeatmapRadiusTransition(const TransitionOptions&);
    TransitionOptions getHeatmapRadiusTransition() const;

    static PropertyValue<float> getDefaultHeatmapWeight();
    const PropertyValue<float>& getHeatmapWeight() const;
    void setHeatmapWeight(const PropertyValue<float>&);
    void setHeatmapWeightTransition(const TransitionOptions&);
    TransitionOptions getHeatmapWeightTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    HeatmapLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
