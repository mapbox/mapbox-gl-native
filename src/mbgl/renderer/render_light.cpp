#include <mbgl/renderer/render_light.hpp>

namespace mbgl {

RenderLight::RenderLight(std::shared_ptr<const style::Light::Impl> impl_)
        : impl(std::move(impl_)) {
}

RenderLight::RenderLight(std::shared_ptr<const style::Light::Impl> impl_, const TransitioningLight transitioning_)
        : impl(std::move(impl_))
        , transitioning(transitioning_) {
}

std::unique_ptr<RenderLight> RenderLight::copy(std::shared_ptr<const style::Light::Impl> impl_) const {
    return std::make_unique<RenderLight>(std::move(impl_), transitioning);
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
