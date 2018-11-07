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
    optional<conversion::Error> setLayoutProperty(const std::string& name, const conversion::Convertible& value) final;
    optional<conversion::Error> setPaintProperty(const std::string& name, const conversion::Convertible& value) final;

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

    static PropertyValue<RasterResamplingType> getDefaultRasterResampling();
    PropertyValue<RasterResamplingType> getRasterResampling() const;
    void setRasterResampling(PropertyValue<RasterResamplingType>);
    void setRasterResamplingTransition(const TransitionOptions&);
    TransitionOptions getRasterResamplingTransition() const;

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

protected:
    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

class RasterLayerFactory : public LayerFactory {
public:
    RasterLayerFactory();
    // LayerFactory overrides.
    ~RasterLayerFactory() override;
    bool supportsType(const std::string& type) const final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const conversion::Convertible& value) final;

    static RasterLayerFactory* get();

private:
    static RasterLayerFactory* instance;
};

} // namespace style
} // namespace mbgl
