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

class RasterLayer : public Layer {
public:
    RasterLayer(const std::string& layerID, const std::string& sourceID);
    ~RasterLayer() final;

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;

    // Paint properties

    static PropertyValue<float> getDefaultRasterBrightnessMax();
    const PropertyValue<float>& getRasterBrightnessMax() const;
    void setRasterBrightnessMax(const PropertyValue<float>&);
    void setRasterBrightnessMaxTransition(const TransitionOptions&);
    TransitionOptions getRasterBrightnessMaxTransition() const;

    static PropertyValue<float> getDefaultRasterBrightnessMin();
    const PropertyValue<float>& getRasterBrightnessMin() const;
    void setRasterBrightnessMin(const PropertyValue<float>&);
    void setRasterBrightnessMinTransition(const TransitionOptions&);
    TransitionOptions getRasterBrightnessMinTransition() const;

    static PropertyValue<float> getDefaultRasterContrast();
    const PropertyValue<float>& getRasterContrast() const;
    void setRasterContrast(const PropertyValue<float>&);
    void setRasterContrastTransition(const TransitionOptions&);
    TransitionOptions getRasterContrastTransition() const;

    static PropertyValue<float> getDefaultRasterFadeDuration();
    const PropertyValue<float>& getRasterFadeDuration() const;
    void setRasterFadeDuration(const PropertyValue<float>&);
    void setRasterFadeDurationTransition(const TransitionOptions&);
    TransitionOptions getRasterFadeDurationTransition() const;

    static PropertyValue<float> getDefaultRasterHueRotate();
    const PropertyValue<float>& getRasterHueRotate() const;
    void setRasterHueRotate(const PropertyValue<float>&);
    void setRasterHueRotateTransition(const TransitionOptions&);
    TransitionOptions getRasterHueRotateTransition() const;

    static PropertyValue<float> getDefaultRasterOpacity();
    const PropertyValue<float>& getRasterOpacity() const;
    void setRasterOpacity(const PropertyValue<float>&);
    void setRasterOpacityTransition(const TransitionOptions&);
    TransitionOptions getRasterOpacityTransition() const;

    static PropertyValue<RasterResamplingType> getDefaultRasterResampling();
    const PropertyValue<RasterResamplingType>& getRasterResampling() const;
    void setRasterResampling(const PropertyValue<RasterResamplingType>&);
    void setRasterResamplingTransition(const TransitionOptions&);
    TransitionOptions getRasterResamplingTransition() const;

    static PropertyValue<float> getDefaultRasterSaturation();
    const PropertyValue<float>& getRasterSaturation() const;
    void setRasterSaturation(const PropertyValue<float>&);
    void setRasterSaturationTransition(const TransitionOptions&);
    TransitionOptions getRasterSaturationTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    RasterLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
