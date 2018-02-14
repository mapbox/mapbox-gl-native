// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/style/color_ramp_property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class HeatmapLayer : public Layer {
public:
    HeatmapLayer(const std::string& layerID, const std::string& sourceID);
    ~HeatmapLayer() final;

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

    static DataDrivenPropertyValue<float> getDefaultHeatmapRadius();
    DataDrivenPropertyValue<float> getHeatmapRadius() const;
    void setHeatmapRadius(DataDrivenPropertyValue<float>);
    void setHeatmapRadiusTransition(const TransitionOptions&);
    TransitionOptions getHeatmapRadiusTransition() const;

    static DataDrivenPropertyValue<float> getDefaultHeatmapWeight();
    DataDrivenPropertyValue<float> getHeatmapWeight() const;
    void setHeatmapWeight(DataDrivenPropertyValue<float>);
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
};

template <>
inline bool Layer::is<HeatmapLayer>() const {
    return getType() == LayerType::Heatmap;
}

} // namespace style
} // namespace mbgl
