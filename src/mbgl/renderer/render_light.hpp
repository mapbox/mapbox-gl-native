#pragma once

#include <mbgl/style/light_impl.hpp>
#include <mbgl/util/immutable.hpp>

namespace mbgl {

class CascadeParameters;
class PropertyEvaluationParameters;

using TransitioningLight = style::LightProperties::Unevaluated;
using EvaluatedLight     = style::LightProperties::PossiblyEvaluated;

class RenderLight {
public:
    RenderLight(Immutable<style::Light::Impl>);

    void transition(const CascadeParameters&);
    void evaluate(const PropertyEvaluationParameters&);
    bool hasTransition() const;

    const EvaluatedLight& getEvaluated() const;

    Immutable<style::Light::Impl> impl;

private:
    TransitioningLight transitioning;
    EvaluatedLight evaluated;
};

} // namespace mbgl
