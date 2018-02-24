#include <mbgl/style/expression/equals.hpp>

namespace mbgl {
namespace style {
namespace expression {

Equals::Equals(std::unique_ptr<Expression> lhs_, std::unique_ptr<Expression> rhs_, bool negate_)
    : Expression(type::Boolean),
      lhs(std::move(lhs_)),
      rhs(std::move(rhs_)),
      negate(negate_) {
}

EvaluationResult Equals::evaluate(const EvaluationContext& params) const {
    EvaluationResult lhsResult = lhs->evaluate(params);
    if (!lhsResult) return lhsResult;

    EvaluationResult rhsResult = rhs->evaluate(params);
    if (!rhsResult) return lhsResult;

    bool result = *lhsResult == *rhsResult;
    if (negate) {
        result = !result;
    }
    return result;
}

void Equals::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*lhs);
    visit(*rhs);
}

bool Equals::operator==(const Expression& e) const {
    if (auto eq = dynamic_cast<const Equals*>(&e)) {
        return eq->negate == negate && *eq->lhs == *lhs && *eq->rhs == *rhs;
    }
    return false;
}

std::vector<optional<Value>> Equals::possibleOutputs() const {
    return {{ true }, { false }};
}

static bool isComparableType(const type::Type& type) {
    return type == type::String ||
        type == type::Number ||
        type == type::Boolean ||
        type == type::Null;
}

using namespace mbgl::style::conversion;
ParseResult Equals::parse(const Convertible& value, ParsingContext& ctx) {
    std::size_t length = arrayLength(value);

    if (length != 3) {
        ctx.error("Expected two arguments.");
        return ParseResult();
    }

    bool negate = toString(arrayMember(value, 0)) == std::string("!=");

    ParseResult lhs = ctx.parse(arrayMember(value, 1), 1, {type::Value});
    if (!lhs) return ParseResult();

    ParseResult rhs = ctx.parse(arrayMember(value, 2), 2, {type::Value});
    if (!rhs) return ParseResult();

    type::Type lhsType = (*lhs)->getType();
    type::Type rhsType = (*rhs)->getType();

    if (!isComparableType(lhsType) && !isComparableType(rhsType)) {
        ctx.error("Expected at least one argument to be a string, number, boolean, or null, but found (" +
            toString(lhsType) + ", " + toString(rhsType) + ") instead.");
        return ParseResult();
    }

    if (lhsType != rhsType && lhsType != type::Value && rhsType != type::Value) {
        ctx.error("Cannot compare " + toString(lhsType) + " and " + toString(rhsType) + ".");
        return ParseResult();
    }

    return ParseResult(std::make_unique<Equals>(std::move(*lhs), std::move(*rhs), negate));
}

} // namespace expression
} // namespace style
} // namespace mbgl
