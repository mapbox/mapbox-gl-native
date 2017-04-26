#include <mbgl/renderer/render_light.hpp>

namespace mbgl {

RenderLight::RenderLight(Immutable<style::Light::Impl> impl_)
    : impl(std::move(impl_)) {
}

void RenderLight::transition(const CascadeParameters& parameters) {
    transitioning = TransitioningLight(impl->properties, std::move(transitioning), parameters);
}

void RenderLight::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = EvaluatedLight(transitioning, parameters);
}

bool RenderLight::hasTransition() const {
    return transitioning.hasTransition();
}

const EvaluatedLight& RenderLight::getEvaluated() const {
    return evaluated;
}

} // namespace mbgl
