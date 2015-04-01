#include <mbgl/style/applied_class_properties.hpp>

namespace mbgl {

AppliedClassProperty::AppliedClassProperty(ClassID class_id, TimePoint begin_, TimePoint end_, const PropertyValue &value_)
    : name(class_id),
    begin(begin_),
    end(end_),
    value(value_) {}

// Returns the ID of the most recent
ClassID AppliedClassProperties::mostRecent() const {
    return properties.size() ? properties.back().name : ClassID::Fallback;
}

void AppliedClassProperties::add(ClassID class_id, TimePoint begin, TimePoint end, const PropertyValue &value) {
    properties.emplace_back(class_id, begin, end, value);
}

bool AppliedClassProperties::hasTransitions() const {
    return properties.size() > 1;
}

// Erase all items in the property list that are before a completed transition.
// Then, if the only remaining property is a Fallback value, remove it too.
void AppliedClassProperties::cleanup(TimePoint now) {
    // Iterate backwards, but without using the rbegin/rend interface since we need forward
    // iterators to use .erase().
    for (auto it = properties.end(), begin = properties.begin(); it != begin;) {
        // If the property is finished, break iteration and delete all remaining items.
        if ((--it)->end <= now) {
            // Removes all items that precede the current iterator, but *not* the element currently
            // pointed to by the iterator. This preserves the last completed transition as the
            // first element in the property list.
            properties.erase(begin, it);

            // Also erase the pivot element if it's a fallback value. This means we can remove the
            // entire applied properties object as well, because we already have the fallback
            // value set as the default.
            if (it->name == ClassID::Fallback) {
                properties.erase(it);
            }
            break;
        }
    }
}

bool AppliedClassProperties::empty() const {
    return properties.empty();
}

}
