#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/in.hpp>
#include <mbgl/style/expression/type.hpp>
#include <mbgl/util/string.hpp>

#include <cstring>

namespace mbgl {
namespace style {
namespace expression {

namespace {
bool isComparableType(const type::Type& type) {
    return type == type::Boolean || type == type::String || type == type::Number || type == type::Null ||
           type == type::Value;
}

bool isComparableRuntimeType(const type::Type& type) {
    return type == type::Boolean || type == type::String || type == type::Number || type == type::Null;
}

bool isSearchableType(const type::Type& type) {
    return type == type::String || type.is<type::Array>() || type == type::Null || type == type::Value;
}

bool isSearchableRuntimeType(const type::Type& type) {
    return type == type::String || type.is<type::Array>() || type == type::Null;
}
} // namespace

In::In(std::unique_ptr<Expression> needle_, std::unique_ptr<Expression> haystack_)
    : Expression(Kind::In, type::Boolean), needle(std::move(needle_)), haystack(std::move(haystack_)) {
    assert(isComparableType(needle->getType()));
    assert(isSearchableType(haystack->getType()));
}

EvaluationResult In::evaluate(const EvaluationContext& params) const {
    const EvaluationResult evaluatedHaystack = haystack->evaluate(params);
    if (!evaluatedHaystack) {
        return evaluatedHaystack.error();
    }

    const EvaluationResult evaluatedNeedle = needle->evaluate(params);
    if (!evaluatedNeedle) {
        return evaluatedNeedle.error();
    }

    type::Type evaluatedNeedleType = typeOf(*evaluatedNeedle);
    if (!isComparableRuntimeType(evaluatedNeedleType)) {
        return EvaluationError{"Expected first argument to be of type boolean, string or number, but found " +
                               toString(evaluatedNeedleType) + " instead."};
    }

    type::Type evaluatedHaystackType = typeOf(*evaluatedHaystack);
    if (!isSearchableRuntimeType(evaluatedHaystackType)) {
        return EvaluationError{"Expected second argument to be of type array or string, but found " +
                               toString(evaluatedHaystackType) + " instead."};
    }

    if (evaluatedNeedleType == type::Null || evaluatedHaystackType == type::Null) {
        return EvaluationResult(false);
    }

    if (evaluatedHaystackType == type::String) {
        const auto haystackString = evaluatedHaystack->get<std::string>();
        const auto needleString = toString(*evaluatedNeedle);
        return EvaluationResult(haystackString.find(needleString) != std::string::npos);
    } else {
        const auto haystackArray = evaluatedHaystack->get<std::vector<Value>>();
        return EvaluationResult(std::find(haystackArray.begin(), haystackArray.end(), *evaluatedNeedle) !=
                                haystackArray.end());
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
    if (!needle) return ParseResult();

    ParseResult haystack = ctx.parse(arrayMember(value, 2), 2, {type::Value});
    if (!haystack) return ParseResult();

    type::Type needleType = (*needle)->getType();
    type::Type haystackType = (*haystack)->getType();

    if (!isComparableType(needleType)) {
        ctx.error("Expected first argument to be of type boolean, string or number, but found " + toString(needleType) +
                  " instead.");
        return ParseResult();
    }

    if (!isSearchableType(haystackType)) {
        ctx.error("Expected second argument to be of type array or string, but found " + toString(haystackType) +
                  " instead.");
        return ParseResult();
    }
    return ParseResult(std::make_unique<In>(std::move(*needle), std::move(*haystack)));
}

bool In::operator==(const Expression& e) const {
    if (e.getKind() == Kind::In) {
        auto rhs = static_cast<const In*>(&e);
        return *needle == *(rhs->needle) && *haystack == *(rhs->haystack);
    }
    return false;
}

std::vector<optional<Value>> In::possibleOutputs() const {
    return {{true}, {false}};
}

std::string In::getOperator() const {
    return "in";
}

} // namespace expression
} // namespace style
} // namespace mbgl
