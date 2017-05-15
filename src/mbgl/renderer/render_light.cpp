#include <mbgl/renderer/render_light.hpp>

namespace mbgl {

RenderLight::RenderLight(Immutable<style::Light::Impl> impl_)
    : impl(std::move(impl_)) {
}

void RenderLight::transition(const CascadeParameters& parameters) {
    transitioning = impl->properties.cascade(parameters, std::move(transitioning));
}

void RenderLight::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = transitioning.evaluate(parameters);
}

bool RenderLight::hasTransition() const {
    return transitioning.hasTransition();
}

const EvaluatedLight& RenderLight::getEvaluated() const {
    return evaluated;
}

} // namespace mbgl
