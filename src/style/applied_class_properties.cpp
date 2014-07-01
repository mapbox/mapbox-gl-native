#include <llmr/style/applied_class_properties.hpp>

namespace llmr {

AppliedClassProperty::AppliedClassProperty(ClassID class_id, timestamp begin, timestamp end, const PropertyValue &value)
    : name(class_id),
    begin(begin),
    end(end),
    value(value) {}

// Returns thie ID of the most recent
ClassID AppliedClassProperties::mostRecent() const {
    return properties.size() ? properties.back().name : ClassID::Fallback;
}

void AppliedClassProperties::add(ClassID class_id, timestamp begin, timestamp end, const PropertyValue &value) {
    properties.emplace_back(class_id, begin, end, value);
}

bool AppliedClassProperties::hasTransitions() const {
    return properties.size() > 1;
}

// Erase all items in the property list that are before a completed transition.
// Then, if the only remaining property is a Fallback value, remove it too.
void AppliedClassProperties::cleanup(timestamp now) {
    for (auto it = properties.end(), begin = properties.begin(); it != begin;) {
        // If the property is finished, break iteration and delete all remaining items.
        if ((--it)->end <= now) {
            properties.erase(begin, it);
            break;
        }
    }
}

}