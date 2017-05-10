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

class RasterLayer : public Layer {
public:
    RasterLayer(const std::string& layerID, const std::string& sourceID);
    ~RasterLayer() final;

    // Source
    const std::string& getSourceID() const;

    // Visibility
    void setVisibility(VisibilityType) final;

    // Zoom range
    void setMinZoom(float) final;
    void setMaxZoom(float) final;

    // Paint properties

    static PropertyValue<float> getDefaultRasterOpacity();
    PropertyValue<float> getRasterOpacity() const;
    void setRasterOpacity(PropertyValue<float>);
    void setRasterOpacityTransition(const TransitionOptions&);
    TransitionOptions getRasterOpacityTransition() const;

    static PropertyValue<float> getDefaultRasterHueRotate();
    PropertyValue<float> getRasterHueRotate() const;
    void setRasterHueRotate(PropertyValue<float>);
    void setRasterHueRotateTransition(const TransitionOptions&);
    TransitionOptions getRasterHueRotateTransition() const;

    static PropertyValue<float> getDefaultRasterBrightnessMin();
    PropertyValue<float> getRasterBrightnessMin() const;
    void setRasterBrightnessMin(PropertyValue<float>);
    void setRasterBrightnessMinTransition(const TransitionOptions&);
    TransitionOptions getRasterBrightnessMinTransition() const;

    static PropertyValue<float> getDefaultRasterBrightnessMax();
    PropertyValue<float> getRasterBrightnessMax() const;
    void setRasterBrightnessMax(PropertyValue<float>);
    void setRasterBrightnessMaxTransition(const TransitionOptions&);
    TransitionOptions getRasterBrightnessMaxTransition() const;

    static PropertyValue<float> getDefaultRasterSaturation();
    PropertyValue<float> getRasterSaturation() const;
    void setRasterSaturation(PropertyValue<float>);
    void setRasterSaturationTransition(const TransitionOptions&);
    TransitionOptions getRasterSaturationTransition() const;

    static PropertyValue<float> getDefaultRasterContrast();
    PropertyValue<float> getRasterContrast() const;
    void setRasterContrast(PropertyValue<float>);
    void setRasterContrastTransition(const TransitionOptions&);
    TransitionOptions getRasterContrastTransition() const;

    static PropertyValue<float> getDefaultRasterFadeDuration();
    PropertyValue<float> getRasterFadeDuration() const;
    void setRasterFadeDuration(PropertyValue<float>);
    void setRasterFadeDurationTransition(const TransitionOptions&);
    TransitionOptions getRasterFadeDurationTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    RasterLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;
};

template <>
inline bool Layer::is<RasterLayer>() const {
    return getType() == LayerType::Raster;
}

} // namespace style
} // namespace mbgl
