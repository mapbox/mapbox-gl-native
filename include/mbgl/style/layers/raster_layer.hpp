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

    // Paint properties

    static PropertyValue<float> getDefaultRasterOpacity();
    PropertyValue<float> getRasterOpacity(const optional<std::string>& klass = {}) const;
    void setRasterOpacity(PropertyValue<float>, const optional<std::string>& klass = {});
    void setRasterOpacityTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getRasterOpacityTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<float> getDefaultRasterHueRotate();
    PropertyValue<float> getRasterHueRotate(const optional<std::string>& klass = {}) const;
    void setRasterHueRotate(PropertyValue<float>, const optional<std::string>& klass = {});
    void setRasterHueRotateTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getRasterHueRotateTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<float> getDefaultRasterBrightnessMin();
    PropertyValue<float> getRasterBrightnessMin(const optional<std::string>& klass = {}) const;
    void setRasterBrightnessMin(PropertyValue<float>, const optional<std::string>& klass = {});
    void setRasterBrightnessMinTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getRasterBrightnessMinTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<float> getDefaultRasterBrightnessMax();
    PropertyValue<float> getRasterBrightnessMax(const optional<std::string>& klass = {}) const;
    void setRasterBrightnessMax(PropertyValue<float>, const optional<std::string>& klass = {});
    void setRasterBrightnessMaxTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getRasterBrightnessMaxTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<float> getDefaultRasterSaturation();
    PropertyValue<float> getRasterSaturation(const optional<std::string>& klass = {}) const;
    void setRasterSaturation(PropertyValue<float>, const optional<std::string>& klass = {});
    void setRasterSaturationTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getRasterSaturationTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<float> getDefaultRasterContrast();
    PropertyValue<float> getRasterContrast(const optional<std::string>& klass = {}) const;
    void setRasterContrast(PropertyValue<float>, const optional<std::string>& klass = {});
    void setRasterContrastTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getRasterContrastTransition(const optional<std::string>& klass = {}) const;

    static PropertyValue<float> getDefaultRasterFadeDuration();
    PropertyValue<float> getRasterFadeDuration(const optional<std::string>& klass = {}) const;
    void setRasterFadeDuration(PropertyValue<float>, const optional<std::string>& klass = {});
    void setRasterFadeDurationTransition(const TransitionOptions&, const optional<std::string>& klass = {});
    TransitionOptions getRasterFadeDurationTransition(const optional<std::string>& klass = {}) const;

    // Private implementation

    class Impl;
    Impl* const impl;

    RasterLayer(const Impl&);
    RasterLayer(const RasterLayer&) = delete;
};

template <>
inline bool Layer::is<RasterLayer>() const {
    return type == LayerType::Raster;
}

} // namespace style
} // namespace mbgl
