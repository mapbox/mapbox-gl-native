#include <llmr/style/applied_class_properties.hpp>

namespace llmr {

AppliedClassProperty::AppliedClassProperty(ClassID class_id, timestamp begin, timestamp end, const PropertyValue &value)
    : name(class_id),
    begin(begin),
    end(end),
    value(value) {}

}
