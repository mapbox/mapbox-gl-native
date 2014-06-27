#include <llmr/style/property_fallback.hpp>

namespace llmr {

const std::map<PropertyKey, PropertyValue> PropertyFallbackValue::properties = {
    { PropertyKey::LineEnabled, true },
    { PropertyKey::LineOpacity, 1.0f },
    { PropertyKey::LineColor, Color({{ 0, 0, 0, 1 }}) },
};

const PropertyValue PropertyFallbackValue::defaultProperty;

}
