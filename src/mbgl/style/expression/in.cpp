#include <string.h>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/in.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

EvaluationResult In::evaluate(const EvaluationContext& params) const {
    const EvaluationResult evaluatedNeedle = needle->evaluate(params);
    const EvaluationResult evaluatedHeystack = haystack->evaluate(params);

    if (!evaluatedNeedle) {
        return evaluatedNeedle.error();
    }
    if (!evaluatedHeystack) {
        return evaluatedHeystack.error();
    }

    type::Type evaluatedNeedleType = typeOf(*evaluatedNeedle);
    if (!isComparableRuntimeValue(evaluatedNeedleType)) {
        return EvaluationError{"Expected first argument to be of type boolean, string or number, but found " +
                               toString(evaluatedNeedleType) + " instead."};
    }

    type::Type evaluatedHeystackType = typeOf(*evaluatedHeystack);
    if (!isSearchableRuntimeValue(evaluatedHeystackType)) {
        return EvaluationError{"Expected second argument to be of type array or string, but found " +
                               toString(evaluatedHeystackType) + " instead."};
    }

    if (evaluatedNeedleType == type::Null || evaluatedHeystackType == type::Null) {
        return EvaluationResult(false);
    }

    if (evaluatedHeystackType == type::String) {
        const auto heystackString = evaluatedHeystack->get<std::string>();
        std::string needleValue = "";
        if (evaluatedNeedleType == type::Boolean) {
            needleValue = evaluatedNeedle->get<bool>() ? "true" : " false";
        } else if (evaluatedNeedleType == type::String) {
            needleValue = evaluatedNeedle->get<std::string>();
        } else if (evaluatedNeedleType == type::Number) {
            needleValue = std::to_string(evaluatedNeedle->get<double>());
            needleValue.erase(needleValue.find_last_not_of('0') + 1, std::string::npos);
            needleValue.erase(needleValue.find_last_not_of('.') + 1, std::string::npos);
        }
        return EvaluationResult(heystackString.find(needleValue) != std::string::npos);
    } else {
        const auto heystackArray = evaluatedHeystack->get<std::vector<Value>>();

        bool result = false;
        if (evaluatedNeedleType == type::Boolean) {
            auto needleValue = evaluatedNeedle->get<bool>();
            result = find(heystackArray.begin(), heystackArray.end(), needleValue) != heystackArray.end();
        } else if (evaluatedNeedleType == type::String) {
            auto needleValue = evaluatedNeedle->get<std::string>();
            result = find(heystackArray.begin(), heystackArray.end(), needleValue) != heystackArray.end();
        } else if (evaluatedNeedleType == type::Number) {
            auto needleValue = evaluatedNeedle->get<double>();
            result = find(heystackArray.begin(), heystackArray.end(), needleValue) != heystackArray.end();
        }
        return EvaluationResult(result);
    }
}

void In::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*needle);
    visit(*haystack);
}

using namespace mbgl::style::conversion;
ParseResult In::parse(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));

    std::size_t length = arrayLength(value);
    if (length != 3) {
        ctx.error("Expected 2 arguments, but found " + util::toString(length - 1) + " instead.");
        return ParseResult();
    }

    ParseResult needle = ctx.parse(arrayMember(value, 1), 1, {type::Value});
    ParseResult haystack = ctx.parse(arrayMember(value, 2), 2, {type::Value});

    if (!needle || !haystack) return ParseResult();

    type::Type needleType = (*needle)->getType();
    type::Type haystackType = (*haystack)->getType();

    if (!isComparableType(needleType)) {
        ctx.error("Expected first argument to be of type boolean, string or number, but found " + toString(needleType) +
                  " instead.");
        return ParseResult();
    }

    return ParseResult(std::make_unique<In>(std::move(*needle), std::move(*haystack)));
}

bool In::isComparableType(type::Type type) {
    return type == type::Boolean || type == type::String || type == type::Number || type == type::Null ||
           type == type::Value;
}

bool In::isComparableRuntimeValue(type::Type type) {
    return type == type::Boolean || type == type::String || type == type::Number || type == type::Null;
}

bool In::isSearchableRuntimeValue(type::Type type) {
    return type == type::String || type.is<type::Array>() || type == type::Null;
}

} // namespace expression
} // namespace style
} // namespace mbgl
