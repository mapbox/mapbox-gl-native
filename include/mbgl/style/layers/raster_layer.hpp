// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>

#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class RasterLayer : public Layer {
public:
    RasterLayer(const std::string& layerID, const std::string& sourceID);
    ~RasterLayer() final;

    // Source
    const std::string& getSourceID() const;

    // Paint properties

    PropertyValue<float> getRasterOpacity() const;
    void setRasterOpacity(PropertyValue<float>);

    PropertyValue<float> getRasterHueRotate() const;
    void setRasterHueRotate(PropertyValue<float>);

    PropertyValue<float> getRasterBrightnessMin() const;
    void setRasterBrightnessMin(PropertyValue<float>);

    PropertyValue<float> getRasterBrightnessMax() const;
    void setRasterBrightnessMax(PropertyValue<float>);

    PropertyValue<float> getRasterSaturation() const;
    void setRasterSaturation(PropertyValue<float>);

    PropertyValue<float> getRasterContrast() const;
    void setRasterContrast(PropertyValue<float>);

    PropertyValue<float> getRasterFadeDuration() const;
    void setRasterFadeDuration(PropertyValue<float>);

    // Private implementation

    class Impl;
    Impl* const impl;

    RasterLayer(const Impl&);
    RasterLayer(const RasterLayer&) = delete;
};

template <>
inline bool Layer::is<RasterLayer>() const {
    return type == Type::Raster;
}

} // namespace style
} // namespace mbgl
