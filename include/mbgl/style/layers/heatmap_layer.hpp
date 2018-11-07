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
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

    // Paint properties

    static PropertyValue<float> getDefaultHeatmapRadius();
    PropertyValue<float> getHeatmapRadius() const;
    void setHeatmapRadius(PropertyValue<float>);
    void setHeatmapRadiusTransition(const TransitionOptions&);
    TransitionOptions getHeatmapRadiusTransition() const;

    static PropertyValue<float> getDefaultHeatmapWeight();
    PropertyValue<float> getHeatmapWeight() const;
    void setHeatmapWeight(PropertyValue<float>);
    void setHeatmapWeightTransition(const TransitionOptions&);
    TransitionOptions getHeatmapWeightTransition() const;

    static PropertyValue<float> getDefaultHeatmapIntensity();
    PropertyValue<float> getHeatmapIntensity() const;
    void setHeatmapIntensity(PropertyValue<float>);
    void setHeatmapIntensityTransition(const TransitionOptions&);
    TransitionOptions getHeatmapIntensityTransition() const;

    static ColorRampPropertyValue getDefaultHeatmapColor();
    ColorRampPropertyValue getHeatmapColor() const;
    void setHeatmapColor(ColorRampPropertyValue);
    void setHeatmapColorTransition(const TransitionOptions&);
    TransitionOptions getHeatmapColorTransition() const;

    static PropertyValue<float> getDefaultHeatmapOpacity();
    PropertyValue<float> getHeatmapOpacity() const;
    void setHeatmapOpacity(PropertyValue<float>);
    void setHeatmapOpacityTransition(const TransitionOptions&);
    TransitionOptions getHeatmapOpacityTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    HeatmapLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

class HeatmapLayerFactory : public LayerFactory {
public:
    HeatmapLayerFactory();
    // LayerFactory overrides.
    ~HeatmapLayerFactory() override;
    bool supportsType(const std::string& type) const noexcept final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const conversion::Convertible& value) final;

    static HeatmapLayerFactory* get() noexcept;

private:
    static HeatmapLayerFactory* instance;
};

} // namespace style
} // namespace mbgl
