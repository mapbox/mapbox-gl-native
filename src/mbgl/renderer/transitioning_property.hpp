#pragma once

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/util/interpolate.hpp>

#include <utility>

namespace mbgl {

template <class Value>
class TransitioningProperty {
public:
    TransitioningProperty() = default;

    TransitioningProperty(Value value_,
                          TransitioningProperty<Value> prior_,
                          style::TransitionOptions transition,
                          TimePoint now)
        : begin(now + transition.delay.value_or(Duration::zero())),
          end(begin + transition.duration.value_or(Duration::zero())),
          value(std::move(value_)) {
        if (transition.isDefined()) {
            prior = { std::move(prior_) };
        }
    }

    template <class Evaluator>
    auto evaluate(const Evaluator& evaluator, TimePoint now) {
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
    optional<mapbox::util::recursive_wrapper<TransitioningProperty<Value>>> prior;
    TimePoint begin;
    TimePoint end;
    Value value;
};

} // namespace mbgl
