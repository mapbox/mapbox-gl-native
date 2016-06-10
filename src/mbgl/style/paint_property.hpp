#pragma once

#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/cascade_parameters.hpp>
#include <mbgl/style/calculation_parameters.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <map>
#include <utility>

namespace mbgl {
namespace style {

template <class T, template <class S> class Evaluator = PropertyEvaluator>
class PaintProperty {
public:
    using Result = typename Evaluator<T>::ResultType;

    explicit PaintProperty(T defaultValue_)
        : defaultValue(defaultValue_) {
        values.emplace(ClassID::Fallback, defaultValue_);
    }

    PaintProperty(const PaintProperty& other)
        : values(other.values),
          transitions(other.transitions) {
    }

    const PropertyValue<T>& get() const {
        return values.at(ClassID::Default);
    }

    void set(const PropertyValue<T>& value_) {
        values[ClassID::Default] = value_;
    }

    void parse(const char* name, const JSValue& layer) {
        mbgl::util::erase_if(values, [] (const auto& p) { return p.first != ClassID::Fallback; });

        std::string transitionName = { name };
        transitionName += "-transition";

        for (auto it = layer.MemberBegin(); it != layer.MemberEnd(); ++it) {
            const std::string paintName { it->name.GetString(), it->name.GetStringLength() };
            if (paintName.compare(0, 5, "paint") != 0)
                continue;

            bool isClass = paintName.compare(0, 6, "paint.") == 0;
            if (isClass && paintName.length() <= 6)
                continue;

            ClassID classID = isClass ? ClassDictionary::Get().lookup(paintName.substr(6)) : ClassID::Default;

            if (it->value.HasMember(name)) {
                if (auto v = parseProperty<T>(name, it->value[name])) {
                    values.emplace(classID, v);
                }
            }

            if (it->value.HasMember(transitionName.c_str())) {
                if (auto v = parseTransitionOptions(name, it->value[transitionName.c_str()])) {
                    transitions.emplace(classID, *v);
                }
            }
        }
    }

    void cascade(const CascadeParameters& params) {
        const bool overrideTransition = !params.transition.delay && !params.transition.duration;
        Duration delay = params.transition.delay.value_or(Duration::zero());
        Duration duration = params.transition.duration.value_or(Duration::zero());

        for (const auto classID : params.classes) {
            if (values.find(classID) == values.end())
                continue;

            if (overrideTransition && transitions.find(classID) != transitions.end()) {
                const TransitionOptions& transition = transitions[classID];
                if (transition.delay) delay = *transition.delay;
                if (transition.duration) duration = *transition.duration;
            }

            cascaded = std::make_unique<CascadedValue>(std::move(cascaded),
                                                       params.now + delay,
                                                       params.now + delay + duration,
                                                       values.at(classID));

            break;
        }

        assert(cascaded);
    }

    bool calculate(const CalculationParameters& parameters) {
        assert(cascaded);
        Evaluator<T> evaluator(parameters, defaultValue);
        value = cascaded->calculate(evaluator, parameters.now);
        return cascaded->prior.operator bool();
    }

    // TODO: remove / privatize
    operator T() const { return value; }
    Result value;

private:
    T defaultValue;
    std::map<ClassID, PropertyValue<T>> values;
    std::map<ClassID, TransitionOptions> transitions;

    struct CascadedValue {
        CascadedValue(std::unique_ptr<CascadedValue> prior_,
                      TimePoint begin_,
                      TimePoint end_,
                      PropertyValue<T> value_)
            : prior(std::move(prior_)),
              begin(std::move(begin_)),
              end(std::move(end_)),
              value(std::move(value_)) {
        }

        Result calculate(const Evaluator<T>& evaluator, const TimePoint& now) {
            Result finalValue = PropertyValue<T>::visit(value, evaluator);
            if (!prior) {
                // No prior value.
                return finalValue;
            } else if (now >= end) {
                // Transition from prior value is now complete.
                prior.reset();
                return finalValue;
            } else {
                // Interpolate between recursively-calculated prior value and final.
                float t = std::chrono::duration<float>(now - begin) / (end - begin);
                return util::interpolate(prior->calculate(evaluator, now), finalValue, util::DEFAULT_TRANSITION_EASE.solve(t, 0.001));
            }
        }

        std::unique_ptr<CascadedValue> prior;
        TimePoint begin;
        TimePoint end;
        PropertyValue<T> value;
    };

    std::unique_ptr<CascadedValue> cascaded;
};

} // namespace style
} // namespace mbgl
