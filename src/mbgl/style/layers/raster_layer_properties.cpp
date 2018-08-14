// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {
namespace style {

RasterPaintProperties::Unevaluated RasterPaintProperties::Transitionable::transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
    return Unevaluated {
        rasterOpacity.transition(parameters, std::move(prior.rasterOpacity)),
        rasterHueRotate.transition(parameters, std::move(prior.rasterHueRotate)),
        rasterBrightnessMin.transition(parameters, std::move(prior.rasterBrightnessMin)),
        rasterBrightnessMax.transition(parameters, std::move(prior.rasterBrightnessMax)),
        rasterSaturation.transition(parameters, std::move(prior.rasterSaturation)),
        rasterContrast.transition(parameters, std::move(prior.rasterContrast)),
        rasterResampling.transition(parameters, std::move(prior.rasterResampling)),
        rasterFadeDuration.transition(parameters, std::move(prior.rasterFadeDuration)),
    };
}

RasterPaintProperties::Unevaluated RasterPaintProperties::Transitionable::untransitioned() const {
    return Unevaluated {
        Transitioning<PropertyValue<float>>(rasterOpacity.value),
        Transitioning<PropertyValue<float>>(rasterHueRotate.value),
        Transitioning<PropertyValue<float>>(rasterBrightnessMin.value),
        Transitioning<PropertyValue<float>>(rasterBrightnessMax.value),
        Transitioning<PropertyValue<float>>(rasterSaturation.value),
        Transitioning<PropertyValue<float>>(rasterContrast.value),
        Transitioning<PropertyValue<RasterResamplingType>>(rasterResampling.value),
        Transitioning<PropertyValue<float>>(rasterFadeDuration.value),
    };
}

bool RasterPaintProperties::Transitionable::hasDataDrivenPropertyDifference(const Transitionable& other) const {
    return false
        || rasterOpacity.value.hasDataDrivenPropertyDifference(other.rasterOpacity.value)
        || rasterHueRotate.value.hasDataDrivenPropertyDifference(other.rasterHueRotate.value)
        || rasterBrightnessMin.value.hasDataDrivenPropertyDifference(other.rasterBrightnessMin.value)
        || rasterBrightnessMax.value.hasDataDrivenPropertyDifference(other.rasterBrightnessMax.value)
        || rasterSaturation.value.hasDataDrivenPropertyDifference(other.rasterSaturation.value)
        || rasterContrast.value.hasDataDrivenPropertyDifference(other.rasterContrast.value)
        || rasterResampling.value.hasDataDrivenPropertyDifference(other.rasterResampling.value)
        || rasterFadeDuration.value.hasDataDrivenPropertyDifference(other.rasterFadeDuration.value)
        ;
}

bool RasterPaintProperties::Unevaluated::hasTransition() const {
    return false
        || rasterOpacity.hasTransition()
        || rasterHueRotate.hasTransition()
        || rasterBrightnessMin.hasTransition()
        || rasterBrightnessMax.hasTransition()
        || rasterSaturation.hasTransition()
        || rasterContrast.hasTransition()
        || rasterResampling.hasTransition()
        || rasterFadeDuration.hasTransition()
        ;
}

RasterPaintProperties::PossiblyEvaluated RasterPaintProperties::Unevaluated::evaluate(const PropertyEvaluationParameters& parameters) const {
    return PossiblyEvaluated {
        rasterOpacity.evaluate(typename RasterOpacity::EvaluatorType(parameters, RasterOpacity::defaultValue()), parameters.now),
        rasterHueRotate.evaluate(typename RasterHueRotate::EvaluatorType(parameters, RasterHueRotate::defaultValue()), parameters.now),
        rasterBrightnessMin.evaluate(typename RasterBrightnessMin::EvaluatorType(parameters, RasterBrightnessMin::defaultValue()), parameters.now),
        rasterBrightnessMax.evaluate(typename RasterBrightnessMax::EvaluatorType(parameters, RasterBrightnessMax::defaultValue()), parameters.now),
        rasterSaturation.evaluate(typename RasterSaturation::EvaluatorType(parameters, RasterSaturation::defaultValue()), parameters.now),
        rasterContrast.evaluate(typename RasterContrast::EvaluatorType(parameters, RasterContrast::defaultValue()), parameters.now),
        rasterResampling.evaluate(typename RasterResampling::EvaluatorType(parameters, RasterResampling::defaultValue()), parameters.now),
        rasterFadeDuration.evaluate(typename RasterFadeDuration::EvaluatorType(parameters, RasterFadeDuration::defaultValue()), parameters.now),
    };
}



} // namespace style
} // namespace mbgl
