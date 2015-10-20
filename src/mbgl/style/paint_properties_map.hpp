#ifndef MBGL_PAINT_PROPERTIES_MAP
#define MBGL_PAINT_PROPERTIES_MAP

#include <mbgl/style/applied_class_properties.hpp>
#include <mbgl/style/property_evaluator.hpp>
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/style/property_key.hpp>

#include <mbgl/util/interpolate.hpp>

#include <map>
#include <set>

namespace mbgl {

class ClassProperties;
class PropertyTransition;

class PaintPropertiesMap {
public:
    void cascade(const std::vector<std::string>& classNames,
                 const TimePoint& now,
                 const PropertyTransition& defaultTransition);

    bool hasTransitions() const;
    void removeExpiredTransitions(const TimePoint& now);

    template <typename T>
    void calculate(PropertyKey key, T& target, const StyleCalculationParameters& parameters) {
        auto it = appliedStyle.find(key);
        if (it != appliedStyle.end()) {
            AppliedClassPropertyValues &applied = it->second;
            // Iterate through all properties that we need to apply in order.
            const PropertyEvaluator<T> evaluator(parameters);
            for (auto& property : applied.propertyValues) {
                if (parameters.now >= property.begin) {
                    // We overwrite the current property with the new value.
                    target = mapbox::util::apply_visitor(evaluator, property.value);
                } else {
                    // Do not apply this property because its transition hasn't begun yet.
                }
            }
        }
    }

    template <typename T>
    void calculateTransitioned(PropertyKey key, T& target, const StyleCalculationParameters& parameters) {
        auto it = appliedStyle.find(key);
        if (it != appliedStyle.end()) {
            AppliedClassPropertyValues &applied = it->second;
            // Iterate through all properties that we need to apply in order.
            const PropertyEvaluator<T> evaluator(parameters);
            for (auto& property : applied.propertyValues) {
                if (parameters.now >= property.end) {
                    // We overwrite the current property with the new value.
                    target = mapbox::util::apply_visitor(evaluator, property.value);
                } else if (parameters.now >= property.begin) {
                    // We overwrite the current property partially with the new value.
                    float progress = std::chrono::duration<float>(parameters.now - property.begin) / (property.end - property.begin);
                    target = util::interpolate(target, mapbox::util::apply_visitor(evaluator, property.value), progress);
                    hasPendingTransitions = true;
                } else {
                    // Do not apply this property because its transition hasn't begun yet.
                }
            }
        }
    }

    // Raw parsed paint class to property value map.
    std::map<ClassID, ClassProperties> paints;

private:
    // Applies all properties from a class, if they haven't been applied already.
    void cascadeClass(const ClassID,
                      std::set<PropertyKey>&,
                      const TimePoint&,
                      const PropertyTransition&);

    // For every property, stores a list of applied property values, with
    // optional transition times.
    std::map<PropertyKey, AppliedClassPropertyValues> appliedStyle;

    // Stores whether there are pending transitions to be done on each update.
    bool hasPendingTransitions = false;
};

};

#endif
