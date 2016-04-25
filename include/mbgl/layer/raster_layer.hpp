// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>

namespace mbgl {

class RasterLayer : public Layer {
public:
    RasterLayer(const std::string& layerID);
    ~RasterLayer() final;

    // Source

    void setSource(const std::string& sourceID);
    const std::string& getSourceID() const;

    // Paint properties

    Function<float> getRasterOpacity() const;
    void setRasterOpacity(Function<float>);

    Function<float> getRasterHueRotate() const;
    void setRasterHueRotate(Function<float>);

    Function<float> getRasterBrightnessMin() const;
    void setRasterBrightnessMin(Function<float>);

    Function<float> getRasterBrightnessMax() const;
    void setRasterBrightnessMax(Function<float>);

    Function<float> getRasterSaturation() const;
    void setRasterSaturation(Function<float>);

    Function<float> getRasterContrast() const;
    void setRasterContrast(Function<float>);

    Function<float> getRasterFadeDuration() const;
    void setRasterFadeDuration(Function<float>);

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

} // namespace mbgl
