#pragma once

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/conversion/stringify.hpp>
#include <mbgl/renderer/transition_parameters.hpp>
#include <mbgl/renderer/paint_property_binder.hpp>
#include <mbgl/renderer/property_evaluation_parameters.hpp>
#include <mbgl/renderer/transition_parameters.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/ignore.hpp>

namespace mbgl {

class GeometryTileFeature;

namespace style {

template <class Value>
class Transitioning {
public:
    Transitioning() = default;

    explicit Transitioning(Value value_)
        : value(std::move(value_)) {
    }

    Transitioning(Value value_,
                  Transitioning<Value> prior_,
                  TransitionOptions transition,
                  TimePoint now)
        : begin(now + transition.delay.value_or(Duration::zero())),
          end(begin + transition.duration.value_or(Duration::zero())),
          value(std::move(value_)) {
        if (transition.isDefined()) {
            prior = { std::move(prior_) };
        }
    }

    template <class Evaluator>
    auto evaluate(const Evaluator& evaluator, TimePoint now) const {
        auto finalValue = value.evaluate(evaluator);
        if (!prior) {
            // No prior value.
            return finalValue;
        } else if (now >= end) {
            // Transition from prior value is now complete.
            prior = {};
            return finalValue;
        } else if (value.isDataDriven()) {
            // Transitions to data-driven properties are not supported.
            // We snap immediately to the data-driven value so that, when we perform layout,
            // we see the data-driven function and can use it to populate vertex buffers.
            prior = {};
            return finalValue;
        } else if (now < begin) {
            // Transition hasn't started yet.
            return prior->get().evaluate(evaluator, now);
        } else {
            // Interpolate between recursively-calculated prior value and final.
            float t = std::chrono::duration<float>(now - begin) / (end - begin);
            return util::interpolate(prior->get().evaluate(evaluator, now), finalValue,
                                     util::DEFAULT_TRANSITION_EASE.solve(t, 0.001));
        }
    }

    bool hasTransition() const {
        return bool(prior);
    }

    bool isUndefined() const {
        return value.isUndefined();
    }

    const Value& getValue() const {
        return value;
    }

private:
    mutable optional<mapbox::util::recursive_wrapper<Transitioning<Value>>> prior;
    TimePoint begin;
    TimePoint end;
    Value value;
};

template <class Value>
class Transitionable {
public:
    Value value;
    TransitionOptions options;

    Transitioning<Value> transition(const TransitionParameters& params, Transitioning<Value> prior) const {
        return Transitioning<Value>(value,
                                    std::move(prior),
                                    options.reverseMerge(params.transition),
                                    params.now);
    }
};

class NoProperties {
public:
    class PossiblyEvaluated {};

    class Binders {
    public:
        void populateVertexVectors(const GeometryTileFeature&, std::size_t length);
        void upload(gl::Context&);

        using Attributes = gl::Attributes<>;
        using Uniforms = gl::Uniforms<>;

        using AttributeBindings = typename Attributes::Bindings;
        using UniformValues = typename Uniforms::Values;

        template <class PossiblyEvaluated>
        AttributeBindings attributeBindings(const PossiblyEvaluated&) const {
            return {};
        }

        template <class PossiblyEvaluated>
        UniformValues uniformValues(float, const PossiblyEvaluated&) const {
            return {};
        }
    };
};

} // namespace style
} // namespace mbgl
