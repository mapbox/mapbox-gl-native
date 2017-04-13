#pragma once

#include <mbgl/style/transitioning_property.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/ignore.hpp>
#include <mbgl/util/chrono.hpp>

#include <memory>

namespace mbgl {
namespace style {

template <class Value>
class TransitioningLightProperty {
public:
    using UnevaluatedType = TransitioningProperty<Value>;

    bool isUndefined() const {
        return bool(value);
    }

    const Value& get() const {
        return value;
    }

    template <class Evaluator>
    auto evaluate(const Evaluator& evaluator, TimePoint now) {
        return unevaluated.evaluate(evaluator, now);
    }

    void set(const Value& value_) {
        value = value_;
        unevaluated =
            TransitioningProperty<Value>(std::move(value), unevaluated, transition, Clock::now());
    }

    const TransitionOptions& getTransition() const {
        if (transition) {
            return transition;
        } else {
            static const TransitionOptions staticValue{};
            return staticValue;
        }
    }

    void setTransition(const TransitionOptions& transition_) {
        transition = transition_;
        unevaluated = UnevaluatedType(value, std::move(unevaluated),
                                      transition_.reverseMerge(transition), Clock::now());
    }

    bool hasTransition() const {
        return unevaluated.hasTransition();
    }

private:
    UnevaluatedType unevaluated;
    Value value;
    TransitionOptions transition;
};

template <class T>
class LightProperty {
public:
    using ValueType = PropertyValue<T>;
    using TransitioningType = TransitioningLightProperty<ValueType>;
    using EvaluatorType = PropertyEvaluator<T>;
    using EvaluatedType = T;
};

template <class... Ps>
class LightProperties {
public:
    using Properties = TypeList<Ps...>;

    using EvaluatedTypes = TypeList<typename Ps::EvaluatedType...>;
    using TransitioningTypes = TypeList<typename Ps::TransitioningType...>;

    template <class TypeList>
    using Tuple = IndexedTuple<Properties, TypeList>;

    class Evaluated : public Tuple<EvaluatedTypes> {
    public:
        using Tuple<EvaluatedTypes>::Tuple;
    };

    class Transitioning : public Tuple<TransitioningTypes> {
    public:
        using Tuple<TransitioningTypes>::Tuple;
    };

    template <class P>
    auto get() const {
        return transitioning.template get<P>().get();
    }

    template <class P>
    void set(const typename P::ValueType& value) {
        transitioning.template get<P>().set(value);
    }

    template <class P>
    TransitionOptions getTransition() {
        return transitioning.template get<P>().getTransition();
    }

    template <class P>
    void setTransition(const TransitionOptions& value) {
        transitioning.template get<P>().setTransition(value);
    }

    template <class P>
    auto evaluate(const PropertyEvaluationParameters& parameters) {
        using Evaluator = typename P::EvaluatorType;
        return transitioning.template get<P>().evaluate(Evaluator(parameters, P::defaultValue()),
                                                        parameters.now);
    }

    void evaluate(const PropertyEvaluationParameters& parameters) {
        evaluated = Evaluated{ evaluate<Ps>(parameters)... };
    }

    bool hasTransition() const {
        bool result = false;
        util::ignore({ result |= transitioning.template get<Ps>().hasTransition()... });
        return result;
    }

    Transitioning transitioning;
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
