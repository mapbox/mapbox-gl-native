#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/is_expression.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/boolean_operator.hpp>

namespace mbgl {
namespace style {
    
namespace expression {

bool Formatted::operator==(const Formatted& other) const {
    if (other.sections.size() != sections.size()) {
        return false;
    }
    for (std::size_t i = 0; i < sections.size(); i++) {
        const auto& thisSection = sections.at(i);
        const auto& otherSection = other.sections.at(i);
        if (thisSection.text != otherSection.text ||
            thisSection.fontScale != otherSection.fontScale ||
            thisSection.fontStack != otherSection.fontStack) {
            return false;
        }
    }
    return true;
}
    
    
std::string Formatted::toString() const {
    std::string result;
    for (const auto& section : sections) {
        result += section.text;
    }
    return result;
}
    
} // namespace expression

namespace conversion {
    
using namespace mbgl::style::expression;

optional<Formatted> Converter<Formatted>::operator()(const Convertible& value, Error&) const {
    using namespace mbgl::style::expression;

    auto result = toString(value);
    if (result) {
        return Formatted(result->c_str());
    } else {
        return nullopt;
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl

