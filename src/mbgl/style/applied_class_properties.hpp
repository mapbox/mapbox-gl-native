#ifndef MBGL_STYLE_APPLIED_CLASS_PROPERTIES
#define MBGL_STYLE_APPLIED_CLASS_PROPERTIES

#include <mbgl/style/property_value.hpp>
#include <mbgl/style/class_dictionary.hpp>

#include <list>
#include <chrono>

namespace mbgl {

class AppliedClassProperty {
public:
    AppliedClassProperty(ClassID class_id, std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end, const PropertyValue &value);

public:
    const ClassID name;
    const std::chrono::steady_clock::time_point begin;
    const std::chrono::steady_clock::time_point end;
    const PropertyValue value;
};


class AppliedClassProperties {
public:
    std::list<AppliedClassProperty> properties;

public:
    // Returns the ID of the most recent
    ClassID mostRecent() const;
    void add(ClassID class_id, std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end, const PropertyValue &value);
    bool hasTransitions() const;
    void cleanup(std::chrono::steady_clock::time_point now);
    bool empty() const;
};

}

#endif
