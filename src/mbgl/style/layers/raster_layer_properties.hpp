// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>

namespace mbgl {
namespace style {

struct RasterOpacity : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct RasterHueRotate : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterBrightnessMin : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterBrightnessMax : PaintProperty<float> {
    static float defaultValue() { return 1; }
};

struct RasterSaturation : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterContrast : PaintProperty<float> {
    static float defaultValue() { return 0; }
};

struct RasterResampling : PaintProperty<RasterResamplingType> {
    static RasterResamplingType defaultValue() { return RasterResamplingType::Linear; }
};

struct RasterFadeDuration : PaintProperty<float> {
    static float defaultValue() { return 300; }
};

class RasterPaintProperties {
public:

    class PossiblyEvaluated {
    public:
        float rasterOpacity;
        float rasterHueRotate;
        float rasterBrightnessMin;
        float rasterBrightnessMax;
        float rasterSaturation;
        float rasterContrast;
        RasterResamplingType rasterResampling;
        float rasterFadeDuration;
    };

    class Unevaluated {
    public:
        style::Transitioning<PropertyValue<float>> rasterOpacity;
        style::Transitioning<PropertyValue<float>> rasterHueRotate;
        style::Transitioning<PropertyValue<float>> rasterBrightnessMin;
        style::Transitioning<PropertyValue<float>> rasterBrightnessMax;
        style::Transitioning<PropertyValue<float>> rasterSaturation;
        style::Transitioning<PropertyValue<float>> rasterContrast;
        style::Transitioning<PropertyValue<RasterResamplingType>> rasterResampling;
        style::Transitioning<PropertyValue<float>> rasterFadeDuration;

        bool hasTransition() const;
        PossiblyEvaluated evaluate(const PropertyEvaluationParameters&) const;
    };

    class Transitionable {
    public:
        style::Transitionable<PropertyValue<float>> rasterOpacity;
        style::Transitionable<PropertyValue<float>> rasterHueRotate;
        style::Transitionable<PropertyValue<float>> rasterBrightnessMin;
        style::Transitionable<PropertyValue<float>> rasterBrightnessMax;
        style::Transitionable<PropertyValue<float>> rasterSaturation;
        style::Transitionable<PropertyValue<float>> rasterContrast;
        style::Transitionable<PropertyValue<RasterResamplingType>> rasterResampling;
        style::Transitionable<PropertyValue<float>> rasterFadeDuration;

        Unevaluated transitioned(const TransitionParameters&, Unevaluated&& prior) const;
        Unevaluated untransitioned() const;

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const;
    };
};


} // namespace style
} // namespace mbgl
