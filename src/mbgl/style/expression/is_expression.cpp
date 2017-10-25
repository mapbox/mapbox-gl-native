#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace expression {

bool isExpression(const conversion::Convertible& value) {
    using namespace mbgl::style::conversion;
    
    static std::unordered_set<std::string> specialForms = {
        "literal",
        "match",
        "curve",
        "coalesce",
        "array",
        "let",
        "var",
        "at",
        "string",
        "number",
        "color",
        "boolean",
        "to-string",
        "to-number",
        "to-color",
        "to-boolean",
        "any",
        "all"
    };

    if (!isArray(value) || arrayLength(value) == 0) return false;
    optional<std::string> name = toString(arrayMember(value, 0));
    if (!name) return false;
    
    return (specialForms.find(*name) != specialForms.end()) ||
        (CompoundExpressionRegistry::definitions.find(*name) != CompoundExpressionRegistry::definitions.end());
}


} // namespace expression
} // namespace style
} // namespace mbgl
