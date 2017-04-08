#pragma once

#include <mbgl/style/transitioning_property.hpp>
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/data_driven_property_value.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/cross_faded_property_evaluator.hpp>
#include <mbgl/style/data_driven_property_evaluator.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/style/paint_property_binder.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/ignore.hpp>

#include <utility>

namespace mbgl {

class GeometryTileFeature;

namespace style {

template <class Value>
class CascadingPaintProperty {
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

    template <class TransitioningProperty>
    TransitioningProperty cascade(const CascadeParameters& params, TransitioningProperty prior) const {
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

        return TransitioningProperty(std::move(value),
                                        std::move(prior),
                                        transition.reverseMerge(params.transition),
                                        params.now);
    }

private:
    std::map<ClassID, Value> values;
    std::map<ClassID, TransitionOptions> transitions;
};

template <class T>
class PaintProperty {
public:
    using ValueType = PropertyValue<T>;
    using CascadingType = CascadingPaintProperty<ValueType>;
    using UnevaluatedType = TransitioningProperty<ValueType>;
    using EvaluatorType = PropertyEvaluator<T>;
    using EvaluatedType = T;
    static constexpr bool IsDataDriven = false;
};

template <class T, class A>
class DataDrivenPaintProperty {
public:
    using ValueType = DataDrivenPropertyValue<T>;
    using CascadingType = CascadingPaintProperty<ValueType>;
    using UnevaluatedType = TransitioningProperty<ValueType>;
    using EvaluatorType = DataDrivenPropertyEvaluator<T>;
    using EvaluatedType = PossiblyEvaluatedPropertyValue<T>;
    static constexpr bool IsDataDriven = true;

    using Type = T;
    using Attribute = A;
};

template <class T>
class CrossFadedPaintProperty {
public:
    using ValueType = PropertyValue<T>;
    using CascadingType = CascadingPaintProperty<ValueType>;
    using UnevaluatedType = TransitioningProperty<ValueType>;
    using EvaluatorType = CrossFadedPropertyEvaluator<T>;
    using EvaluatedType = Faded<T>;
    static constexpr bool IsDataDriven = false;
};

template <class P>
struct IsDataDriven : std::integral_constant<bool, P::IsDataDriven> {};

template <class... Ps>
class PaintProperties {
public:
    using Properties = TypeList<Ps...>;
    using DataDrivenProperties = FilteredTypeList<Properties, IsDataDriven>;
    using Binders = PaintPropertyBinders<DataDrivenProperties>;

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

    template <class P>
    void setTransition(const TransitionOptions& value, const optional<std::string>& klass) {
        cascading.template get<P>().setTransition(value, klass);
    }
    
    template <class P>
    auto getTransition(const optional<std::string>& klass) const {
        return cascading.template get<P>().getTransition(klass);
    }

    void cascade(const CascadeParameters& parameters) {
        unevaluated = Unevaluated {
            cascading.template get<Ps>().cascade(parameters,
                std::move(unevaluated.template get<Ps>()))...
        };
    }

    template <class P>
    auto evaluate(const PropertyEvaluationParameters& parameters) {
        using Evaluator = typename P::EvaluatorType;
        return unevaluated.template get<P>()
            .evaluate(Evaluator(parameters, P::defaultValue()), parameters.now);
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
