#pragma once

#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/cross_faded_property_evaluator.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/ignore.hpp>

#include <unordered_map>
#include <utility>

namespace mbgl {
namespace style {

template <class T, class Evaluator>
class UnevaluatedPaintProperty {
public:
    using Result = typename Evaluator::ResultType;

    UnevaluatedPaintProperty() = default;

    UnevaluatedPaintProperty(PropertyValue<T> value_,
                             UnevaluatedPaintProperty<T, Evaluator> prior_,
                             TransitionOptions transition,
                             TimePoint now)
        : begin(now + transition.delay.value_or(Duration::zero())),
          end(begin + transition.duration.value_or(Duration::zero())),
          value(std::move(value_)) {
        if (transition) {
            prior = { std::move(prior_) };
        }
    }

    Result evaluate(const PropertyEvaluationParameters& parameters, T defaultValue) {
        Result finalValue = value.evaluate(Evaluator(parameters, defaultValue));
        if (!prior) {
            // No prior value.
            return finalValue;
        } else if (parameters.now >= end) {
            // Transition from prior value is now complete.
            prior = {};
            return finalValue;
        } else if (parameters.now < begin) {
            // Transition hasn't started yet.
            return prior->get().evaluate(parameters, defaultValue);
        } else {
            // Interpolate between recursively-calculated prior value and final.
            float t = std::chrono::duration<float>(parameters.now - begin) / (end - begin);
            return util::interpolate(prior->get().evaluate(parameters, defaultValue), finalValue, util::DEFAULT_TRANSITION_EASE.solve(t, 0.001));
        }
    }

    bool hasTransition() const {
        return bool(prior);
    }

    bool isUndefined() const {
        return value.isUndefined();
    }

private:
    optional<mapbox::util::recursive_wrapper<UnevaluatedPaintProperty<T, Evaluator>>> prior;
    TimePoint begin;
    TimePoint end;
    PropertyValue<T> value;
};

template <class T>
class CascadingPaintProperty {
public:
    bool isUndefined() const {
        return values.find(ClassID::Default) == values.end();
    }

    const PropertyValue<T>& get(const optional<std::string>& klass) const {
        static const PropertyValue<T> staticValue;
        const auto it = values.find(klass ? ClassDictionary::Get().lookup(*klass) : ClassID::Default);
        return it == values.end() ? staticValue : it->second;
    }

    void set(const PropertyValue<T>& value_, const optional<std::string>& klass) {
        values[klass ? ClassDictionary::Get().lookup(*klass) : ClassID::Default] = value_;
    }

    void setTransition(const TransitionOptions& transition, const optional<std::string>& klass) {
        transitions[klass ? ClassDictionary::Get().lookup(*klass) : ClassID::Default] = transition;
    }

    template <class UnevaluatedPaintProperty>
    UnevaluatedPaintProperty cascade(const CascadeParameters& params, UnevaluatedPaintProperty prior) const {
        TransitionOptions transition;
        PropertyValue<T> value;

        for (const auto classID : params.classes) {
            if (values.find(classID) != values.end()) {
                value = values.at(classID);
                break;
            }
        }

        for (const auto classID : params.classes) {
            if (transitions.find(classID) != transitions.end()) {
                transition = transitions.at(classID).reverseMerge(transition);
                break;
            }
        }

        return UnevaluatedPaintProperty(std::move(value),
                                        std::move(prior),
                                        transition.reverseMerge(params.transition),
                                        params.now);
    }

private:
    std::unordered_map<ClassID, PropertyValue<T>> values;
    std::unordered_map<ClassID, TransitionOptions> transitions;
};

template <class T>
class PaintProperty {
public:
    using ValueType = PropertyValue<T>;
    using CascadingType = CascadingPaintProperty<T>;
    using EvaluatorType = PropertyEvaluator<T>;
    using UnevaluatedType = UnevaluatedPaintProperty<T, EvaluatorType>;
    using EvaluatedType = T;
};

template <class T>
class CrossFadedPaintProperty {
public:
    using ValueType = PropertyValue<T>;
    using CascadingType = CascadingPaintProperty<T>;
    using EvaluatorType = CrossFadedPropertyEvaluator<T>;
    using UnevaluatedType = UnevaluatedPaintProperty<T, EvaluatorType>;
    using EvaluatedType = Faded<T>;
};

template <class... Ps>
class PaintProperties {
public:
    using Properties = TypeList<Ps...>;
    using EvaluatedTypes = TypeList<typename Ps::EvaluatedType...>;
    using UnevaluatedTypes = TypeList<typename Ps::UnevaluatedType...>;
    using CascadingTypes = TypeList<typename Ps::CascadingType...>;

    template <class TypeList>
    using Tuple = IndexedTuple<Properties, TypeList>;

    class Evaluated : public Tuple<EvaluatedTypes> {
    public:
        using Tuple<EvaluatedTypes>::Tuple;
    };

    class Unevaluated : public Tuple<UnevaluatedTypes> {
    public:
        using Tuple<UnevaluatedTypes>::Tuple;
    };

    class Cascading : public Tuple<CascadingTypes> {
    public:
        using Tuple<CascadingTypes>::Tuple;
    };

    template <class P>
    auto get(const optional<std::string>& klass) const {
        return cascading.template get<P>().get(klass);
    }

    template <class P>
    void set(const typename P::ValueType& value, const optional<std::string>& klass) {
        cascading.template get<P>().set(value, klass);
    }

    void cascade(const CascadeParameters& parameters) {
        unevaluated = Unevaluated {
            cascading.template get<Ps>().cascade(parameters,
                std::move(unevaluated.template get<Ps>()))...
        };
    }

    template <class P>
    auto evaluate(const PropertyEvaluationParameters& parameters) {
        return unevaluated.template get<P>().evaluate(parameters, P::defaultValue());
    }

    void evaluate(const PropertyEvaluationParameters& parameters) {
        evaluated = Evaluated {
            evaluate<Ps>(parameters)...
        };
    }

    bool hasTransition() const {
        bool result = false;
        util::ignore({ result |= unevaluated.template get<Ps>().hasTransition()... });
        return result;
    }

    Cascading cascading;
    Unevaluated unevaluated;
    Evaluated evaluated;
};

} // namespace style
} // namespace mbgl
