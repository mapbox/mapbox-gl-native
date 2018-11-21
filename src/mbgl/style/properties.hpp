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

template <class P>
struct IsDataDriven : std::integral_constant<bool, P::IsDataDriven> {};

template <class... Ps>
class Properties {
public:
    /*
        For style properties we implement a two step evaluation process: if you have a zoom level,
        you can evaluate a set of unevaluated property values, producing a set of possibly evaluated
        values, where undefined, constant, or camera function values have been fully evaluated, and
        source or composite function values have not.

        Once you also have a particular feature, you can evaluate that set of possibly evaluated values
        fully, producing a set of fully evaluated values.

        This is in theory maximally efficient in terms of avoiding repeated evaluation of camera
        functions, though it's more of a historical accident than a purposeful optimization.
    */

    using          PropertyTypes = TypeList<Ps...>;
    using    TransitionableTypes = TypeList<typename Ps::TransitionableType...>;
    using       UnevaluatedTypes = TypeList<typename Ps::UnevaluatedType...>;
    using PossiblyEvaluatedTypes = TypeList<typename Ps::PossiblyEvaluatedType...>;
    using         EvaluatedTypes = TypeList<typename Ps::Type...>;

    using DataDrivenProperties = FilteredTypeList<PropertyTypes, IsDataDriven>;
    using Binders = PaintPropertyBinders<DataDrivenProperties>;

    template <class TypeList>
    using Tuple = IndexedTuple<PropertyTypes, TypeList>;

    class Evaluated : public Tuple<EvaluatedTypes> {
    public:
        template <class... Us>
        Evaluated(Us&&... us)
            : Tuple<EvaluatedTypes>(std::forward<Us>(us)...) {
        }
    };

    class PossiblyEvaluated : public Tuple<PossiblyEvaluatedTypes> {
    public:
        template <class... Us>
        PossiblyEvaluated(Us&&... us)
            : Tuple<PossiblyEvaluatedTypes>(std::forward<Us>(us)...) {
        }

        template <class T>
        static T evaluate(float, const GeometryTileFeature&, const T& t, const T&) {
            return t;
        }

        template <class T>
        static T evaluate(float z, const GeometryTileFeature& feature,
                          const PossiblyEvaluatedPropertyValue<T>& v, const T& defaultValue) {
            return v.match(
                [&] (const T& t) {
                    return t;
                },
                [&] (const PropertyExpression<T>& t) {
                    return t.evaluate(z, feature, defaultValue);
                });
        }

        template <class P>
        auto evaluate(float z, const GeometryTileFeature& feature) const {
            return evaluate(z, feature, this->template get<P>(), P::defaultValue());
        }

        Evaluated evaluate(float z, const GeometryTileFeature& feature) const {
            return Evaluated {
                evaluate<Ps>(z, feature)...
            };
        }
    };

    class Unevaluated : public Tuple<UnevaluatedTypes> {
    public:
        template <class... Us>
        Unevaluated(Us&&... us)
            : Tuple<UnevaluatedTypes>(std::forward<Us>(us)...) {
        }

        bool hasTransition() const {
            bool result = false;
            util::ignore({ result |= this->template get<Ps>().hasTransition()... });
            return result;
        }

        template <class P>
        auto evaluate(const PropertyEvaluationParameters& parameters) const {
            using Evaluator = typename P::EvaluatorType;
            return this->template get<P>()
                .evaluate(Evaluator(parameters, P::defaultValue()), parameters.now);
        }

        PossiblyEvaluated evaluate(const PropertyEvaluationParameters& parameters) const {
            return PossiblyEvaluated {
                evaluate<Ps>(parameters)...
            };
        }

        template <class Writer>
        void stringify(Writer& writer) const {
            writer.StartObject();
            util::ignore({ (conversion::stringify<Ps>(writer, this->template get<Ps>()), 0)... });
            writer.EndObject();
        }
    };

    class Transitionable : public Tuple<TransitionableTypes> {
    public:
        template <class... Us>
        Transitionable(Us&&... us)
            : Tuple<TransitionableTypes>(std::forward<Us>(us)...) {
        }

        Unevaluated transitioned(const TransitionParameters& parameters, Unevaluated&& prior) const {
            return Unevaluated {
                this->template get<Ps>()
                    .transition(parameters, std::move(prior.template get<Ps>()))...
            };
        }

        Unevaluated untransitioned() const {
            return Unevaluated {
                typename Ps::UnevaluatedType(this->template get<Ps>().value)...
            };
        }

        bool hasDataDrivenPropertyDifference(const Transitionable& other) const {
            bool result = false;
            util::ignore({ (result |= this->template get<Ps>().value.hasDataDrivenPropertyDifference(other.template get<Ps>().value))... });
            return result;
        }
    };
};

template <class... Ps>
using ConcatenateProperties = typename TypeListConcat<typename Ps::PropertyTypes...>::template ExpandInto<Properties>;

} // namespace style
} // namespace mbgl
