#ifndef MBGL_PAINT_PROPERTY
#define MBGL_PAINT_PROPERTY

#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/function.hpp>
#include <mbgl/style/property_transition.hpp>
#include <mbgl/style/style_cascade_parameters.hpp>
#include <mbgl/style/style_calculation_parameters.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/rapidjson.hpp>

#include <map>
#include <utility>

namespace mbgl {

template <typename T, typename Result = T>
class PaintProperty {
public:
    using Fn = Function<Result>;

    explicit PaintProperty(T fallbackValue)
        : value(fallbackValue) {
        values.emplace(ClassID::Fallback, Fn(fallbackValue));
    }

    PaintProperty(const PaintProperty& other)
        : values(other.values),
          transitions(other.transitions) {
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
                auto v = parseProperty<Fn>(name, it->value[name]);
                if (v) {
                    values.emplace(classID, *v);
                }
            }

            if (it->value.HasMember(transitionName.c_str())) {
                auto v = parseProperty<PropertyTransition>(name, it->value[transitionName.c_str()]);
                if (v) {
                    transitions.emplace(classID, *v);
                }
            }
        }
    }

    void cascade(const StyleCascadeParameters& parameters) {
        Duration delay = *parameters.defaultTransition.delay;
        Duration duration = *parameters.defaultTransition.duration;

        for (auto classID : parameters.classes) {
            if (values.find(classID) == values.end())
                continue;

            if (transitions.find(classID) != transitions.end()) {
                const PropertyTransition& transition = transitions[classID];
                if (transition.delay) delay = *transition.delay;
                if (transition.duration) duration = *transition.duration;
            }

            cascaded = std::make_unique<CascadedValue>(std::move(cascaded),
                                                       parameters.now + delay,
                                                       parameters.now + delay + duration,
                                                       values.at(classID));

            break;
        }

        assert(cascaded);
    }

    bool calculate(const StyleCalculationParameters& parameters) {
        assert(cascaded);
        value = cascaded->calculate(parameters);
        return cascaded->prior.operator bool();
    }

    void operator=(const T& v) { values.emplace(ClassID::Default, Fn(v)); }
    operator T() const { return value; }

    std::map<ClassID, Fn> values;
    std::map<ClassID, PropertyTransition> transitions;

    struct CascadedValue {
        CascadedValue(std::unique_ptr<CascadedValue> prior_,
                      TimePoint begin_,
                      TimePoint end_,
                      Fn value_)
            : prior(std::move(prior_)),
              begin(begin_),
              end(end_),
              value(std::move(value_)) {
        }

        Result calculate(const StyleCalculationParameters& parameters) {
            Result final = value.evaluate(parameters);
            if (!prior) {
                // No prior value.
                return final;
            } else if (parameters.now >= end) {
                // Transition from prior value is now complete.
                prior.reset();
                return final;
            } else {
                // Interpolate between recursively-calculated prior value and final.
                float t = std::chrono::duration<float>(parameters.now - begin) / (end - begin);
                return util::interpolate(prior->calculate(parameters), final, t);
            }
        }

        std::unique_ptr<CascadedValue> prior;
        TimePoint begin;
        TimePoint end;
        Fn value;
    };

    std::unique_ptr<CascadedValue> cascaded;

    Result value;
};

} // namespace mbgl

#endif
