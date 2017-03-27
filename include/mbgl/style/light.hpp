#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/property_evaluation_parameters.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/mat3.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/indexed_tuple.hpp>
#include <mbgl/util/ignore.hpp>

#include <memory>

namespace mbgl {
namespace style {

template <class Value>
class UnevaluatedLightProperty {
public:
    UnevaluatedLightProperty() = default;

    UnevaluatedLightProperty(Value value_,
                             UnevaluatedLightProperty<Value> prior_,
                             TransitionOptions transition,
                             TimePoint now)
        : begin(now + transition.delay.value_or(Duration::zero())),
          end(begin + transition.duration.value_or(Duration::zero())),
          value(std::move(value_)) {
        if (transition) {
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
    optional<mapbox::util::recursive_wrapper<UnevaluatedLightProperty<Value>>> prior;
    TimePoint begin;
    TimePoint end;
    Value value;
};

template <class Value>
class CascadingLightProperty {
public:
    bool isUndefined() const {
        return bool(value);
    }

    const Value& get() const {
        return value;
    }

    void set(const Value& value_) {
        value = value_;
    }

    const TransitionOptions& getTransition() const {
        if (bool(transition)) {
            return transition;
        } else {
            static const TransitionOptions staticValue{};
            return staticValue;
            // TODO write this in one line probably
        }
    }

    void setTransition(const TransitionOptions& transition_) {
        transition = transition_;
    }

    template <class UnevaluatedLightProperty>
    UnevaluatedLightProperty cascade(const CascadeParameters& params,
                                     UnevaluatedLightProperty prior) const {
        TransitionOptions transition_;
        Value value_;

        if (bool(value)) {
            value_ = value;
        }

        if (bool(transition)) {
            transition_ = transition;
        }

        return UnevaluatedLightProperty(std::move(value_), std::move(prior),
                                        transition_.reverseMerge(params.transition), params.now);
    }

private:
    Value value;
    TransitionOptions transition;
};

template <class T>
class LightProperty {
public:
    using ValueType = PropertyValue<T>;
    using CascadingType = CascadingLightProperty<ValueType>;
    using UnevaluatedType = UnevaluatedLightProperty<ValueType>;
    using EvaluatorType = PropertyEvaluator<T>;
    using EvaluatedType = T;
};

template <class... Ps>
class LightProperties {
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
    auto get() const {
        return cascading.template get<P>().get();
    }

    template <class P>
    void set(const typename P::ValueType& value) {
        cascading.template get<P>().set(value);
    }

    template <class P>
    void setTransition(const TransitionOptions& value) {
        cascading.template get<P>().setTransition(value);
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
        return Position{{{ 1.15, 210, 30 }}};
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

class Light : public LightProperties<LightAnchor, LightPosition, LightColor, LightIntensity> {
public:
    void setAnchor(PropertyValue<LightAnchorType> value) {
        if (value == this->getAnchor())
            return;
        this->set<LightAnchor>(value);
        // TODO run observer
    }

    PropertyValue<LightAnchorType> getAnchor() const {
        return this->get<LightAnchor>();
    }

    PropertyValue<LightAnchorType> getDefaultAnchor() const {
        return LightAnchor::defaultValue();
    }

    void setPosition(PropertyValue<Position> value) {
        if (value == this->getPosition())
            return;
        this->set<LightPosition>(value);
        // TODO run observer
    }

    PropertyValue<Position> getPosition() const {
        return this->get<LightPosition>();
    }

    PropertyValue<Position> getDefaultPosition() const {
        return LightPosition::defaultValue();
    }

    void setColor(PropertyValue<Color> value) {
        if (value == this->getColor())
            return;
        this->set<LightColor>(value);
        // TODO run observer
    }

    PropertyValue<Color> getColor() const {
        return this->get<LightColor>();
    }

    PropertyValue<Color> getDefaultColor() const {
        return LightColor::defaultValue();
    }

    void setIntensity(PropertyValue<float> value) {
        if (value == this->getIntensity())
            return;
        this->set<LightIntensity>(value);
        // TODO run observer
    }

    PropertyValue<float> getIntensity() const {
        return this->get<LightIntensity>();
    }

    PropertyValue<float> getDefaultIntensity() const {
        return LightIntensity::defaultValue();
    }
};

} // namespace style
} // namespace mbgl
