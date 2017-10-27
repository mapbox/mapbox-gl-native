#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/conversion.hpp>

namespace mbgl {
namespace style {
namespace expression {

using namespace mbgl::style::conversion;

bool isExpression(const Convertible& value) {
    static std::unordered_set<std::string> specialForms = {
        "all",
        "any",
        "array",
        "at",
        "boolean",
        "case",
        "coalesce",
        "color",
        "curve",
        "let",
        "literal",
        "match",
        "number",
        "string",
        "to-boolean",
        "to-color",
        "to-number",
        "to-string",
        "var",
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
