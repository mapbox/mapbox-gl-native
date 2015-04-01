#ifndef MBGL_STYLE_APPLIED_CLASS_PROPERTIES
#define MBGL_STYLE_APPLIED_CLASS_PROPERTIES

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/class_dictionary.hpp>
#include <mbgl/util/chrono.hpp>

#include <list>

namespace mbgl {

class AppliedClassProperty {
public:
    AppliedClassProperty(ClassID class_id, TimePoint begin, TimePoint end, const PropertyValue &value);

public:
    const ClassID name;
    const TimePoint begin;
    const TimePoint end;
    const PropertyValue value;
};


class AppliedClassProperties {
public:
    std::list<AppliedClassProperty> properties;

public:
    // Returns the ID of the most recent
    ClassID mostRecent() const;
    void add(ClassID class_id, TimePoint begin, TimePoint end, const PropertyValue &value);
    bool hasTransitions() const;
    void cleanup(TimePoint now);
    bool empty() const;
};

}

#endif
