#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <unordered_set>

namespace mbgl {
namespace style {
namespace expression {

using namespace mbgl::style::conversion;

bool isExpression(const Convertible& value) {
    if (!isArray(value) || arrayLength(value) == 0) return false;
    optional<std::string> name = toString(arrayMember(value, 0));
    if (!name) return false;
    
    return isExpression(*name) || CompoundExpression::exists(*name);
}


} // namespace expression
} // namespace style
} // namespace mbgl
