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

template <class Value>
class Cascading {
public:
    bool isUndefined() const {
        return values.find(ClassID::Default) == values.end();
    }

    const Value& get(const optional<std::string>& klass) const {
        static const Value staticValue{};
        const auto it = values.find(klass ? ClassDictionary::Get().lookup(*klass) : ClassID::Default);
        return it == values.end() ? staticValue : it->second;
    }

    void set(const Value& value_, const optional<std::string>& klass) {
        values[klass ? ClassDictionary::Get().lookup(*klass) : ClassID::Default] = value_;
    }

    const TransitionOptions& getTransition(const optional<std::string>& klass) const {
        static const TransitionOptions staticValue{};
        const auto it = transitions.find(klass ? ClassDictionary::Get().lookup(*klass) : ClassID::Default);
        return it == transitions.end() ? staticValue : it->second;
    }

    void setTransition(const TransitionOptions& transition, const optional<std::string>& klass) {
        transitions[klass ? ClassDictionary::Get().lookup(*klass) : ClassID::Default] = transition;
    }

    Transitioning<Value> transition(const TransitionParameters& params, Transitioning<Value> prior) const {
        TransitionOptions transition;
        Value value;

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

        return Transitioning<Value>(std::move(value),
                                    std::move(prior),
                                    transition.reverseMerge(params.transition),
                                    params.now);
    }

private:
    std::map<ClassID, Value> values;
    std::map<ClassID, TransitionOptions> transitions;
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
        using Tuple<EvaluatedTypes>::Tuple;
    };

    class PossiblyEvaluated : public Tuple<PossiblyEvaluatedTypes> {
    public:
        using Tuple<PossiblyEvaluatedTypes>::Tuple;

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
                [&] (const SourceFunction<T>& t) {
                    return t.evaluate(feature, defaultValue);
                },
                [&] (const CompositeFunction<T>& t) {
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
        using Tuple<UnevaluatedTypes>::Tuple;

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
        using Tuple<TransitionableTypes>::Tuple;

        Unevaluated transition(const TransitionParameters& parameters, Unevaluated&& prior) const {
            return Unevaluated {
                this->template get<Ps>()
                    .transition(parameters, std::move(prior.template get<Ps>()))...
            };
        }
    };
};

} // namespace style
} // namespace mbgl
