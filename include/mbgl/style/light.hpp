#pragma once

#include <mbgl/style/transitioning_property.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/ignore.hpp>

#include <memory>

namespace mbgl {
namespace style {

template <class T>
class LightProperty {
public:
    using ValueType = PropertyValue<T>;
    using UnevaluatedType = TransitioningProperty<ValueType>;
    using EvaluatorType = PropertyEvaluator<T>;
    using EvaluatedType = T;

    ValueType value;
    TransitionOptions transition;

    TransitioningProperty<ValueType> cascade(const CascadeParameters& params,
                                             TransitioningProperty<ValueType> prior) const {
        return TransitioningProperty<ValueType>(
            value, std::move(prior), transition.reverseMerge(params.transition), params.now);
    }
};

template <class... Ps>
class LightProperties {
public:
    using Properties = TypeList<Ps...>;

    using EvaluatedTypes = TypeList<typename Ps::EvaluatedType...>;
    using UnevaluatedTypes = TypeList<typename Ps::UnevaluatedType...>;

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

    class Cascading : public Tuple<Properties> {
    public:
        using Tuple<Properties>::Tuple;
    };

    template <class P>
    auto get() const {
        return cascading.template get<P>().value;
    }

    template <class P>
    void set(const typename P::ValueType& value) {
        cascading.template get<P>().value = value;
    }

    template <class P>
    TransitionOptions getTransition() {
        return cascading.template get<P>().transition();
    }

    template <class P>
    void setTransition(const TransitionOptions& value) {
        cascading.template get<P>().transition = value;
    }

    void cascade(const CascadeParameters& parameters) {
        unevaluated = Unevaluated{ cascading.template get<Ps>().cascade(
            parameters, std::move(unevaluated.template get<Ps>()))... };
    }

    template <class P>
    auto evaluate(const PropertyEvaluationParameters& parameters) {
        using Evaluator = typename P::EvaluatorType;
        return unevaluated.template get<P>().evaluate(Evaluator(parameters, P::defaultValue()),
                                                      parameters.now);
    }

    void evaluate(const PropertyEvaluationParameters& parameters) {
        evaluated = Evaluated{ evaluate<Ps>(parameters)... };
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

struct LightAnchor : LightProperty<LightAnchorType> {
    static LightAnchorType defaultValue() {
        return LightAnchorType::Viewport;
    }
};
struct LightPosition : LightProperty<Position> {
    static Position defaultValue() {
        std::array<float, 3> default_ = { { 1.15, 210, 30 } };
        return Position{ { default_ } };
    }
};
struct LightColor : LightProperty<Color> {
    static Color defaultValue() {
        return Color::white();
    }
};
struct LightIntensity : LightProperty<float> {
    static float defaultValue() {
        return 0.5;
    }
};

class Light : public LightProperties<LightAnchor, LightPosition, LightColor, LightIntensity> {};

} // namespace style
} // namespace mbgl
