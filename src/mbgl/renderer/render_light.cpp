#include <mbgl/renderer/render_light.hpp>

namespace mbgl {

RenderLight::RenderLight(const style::Light light_)
        : light(std::move(light_)) {
}

void RenderLight::transition(const CascadeParameters& parameters) {
    transitioning = TransitioningLight(light, std::move(transitioning), parameters);
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
