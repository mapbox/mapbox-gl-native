#ifndef LLMR_STYLE_APPLIED_CLASS_PROPERTIES
#define LLMR_STYLE_APPLIED_CLASS_PROPERTIES

#include <llmr/style/property_value.hpp>
#include <llmr/style/class_dictionary.hpp>
#include <llmr/util/time.hpp>

#include <list>

namespace llmr {

class AppliedClassProperty {
public:
    AppliedClassProperty(ClassID class_id, timestamp begin, timestamp end, const PropertyValue &value);

public:
    const ClassID name;
    const timestamp begin;
    const timestamp end;
    const PropertyValue value;
};


class AppliedClassProperties {
public:
    std::list<AppliedClassProperty> properties;

public:
    // Returns thie ID of the most recent
    ClassID mostRecent() const {
        return properties.size() ? properties.back().name : ClassID::Fallback;
    }

    void add(ClassID class_id, timestamp begin, timestamp end, const PropertyValue &value) {
        properties.emplace_back(class_id, begin, end, value);
    }

    bool hasTransitions() const {
        return properties.size() > 1;
    }
};

}

#endif
