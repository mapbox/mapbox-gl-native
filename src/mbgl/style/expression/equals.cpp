#include <mbgl/style/expression/collator.hpp>
#include <mbgl/style/expression/equals.hpp>

namespace mbgl {
namespace style {
namespace expression {

static bool isComparableType(const type::Type& type) {
    return type == type::String ||
        type == type::Number ||
        type == type::Boolean ||
        type == type::Null;
}

Equals::Equals(std::unique_ptr<Expression> lhs_, std::unique_ptr<Expression> rhs_, optional<std::unique_ptr<Expression>> collator_, bool negate_)
    : Expression(Kind::Equals, type::Boolean),
      lhs(std::move(lhs_)),
      rhs(std::move(rhs_)),
      collator(std::move(collator_)),
      negate(negate_) {
    assert(isComparableType(lhs->getType()) || isComparableType(rhs->getType()));
    assert(lhs->getType() == rhs->getType() || lhs->getType() == type::Value || rhs->getType() == type::Value);
}

EvaluationResult Equals::evaluate(const EvaluationContext& params) const {
    EvaluationResult lhsResult = lhs->evaluate(params);
    if (!lhsResult) return lhsResult;

    EvaluationResult rhsResult = rhs->evaluate(params);
    if (!rhsResult) return lhsResult;

    bool result;
    
    if (collator) {
        auto collatorResult = (*collator)->evaluate(params);
        const Collator& c = collatorResult->get<Collator>();
        result = c.compare(lhsResult->get<std::string>(), rhsResult->get<std::string>()) == 0;
    } else {
       result = *lhsResult == *rhsResult;
    }
    if (negate) {
        result = !result;
    }
    return result;
}

void Equals::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*lhs);
    visit(*rhs);
    if (collator) {
        visit(**collator);
    }
}

bool Equals::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Equals) {
        auto eq = static_cast<const Equals*>(&e);
        return eq->negate == negate && *eq->lhs == *lhs && *eq->rhs == *rhs;
    }
    return false;
}

std::vector<optional<Value>> Equals::possibleOutputs() const {
    return {{ true }, { false }};
}

using namespace mbgl::style::conversion;
ParseResult Equals::parse(const Convertible& value, ParsingContext& ctx) {
    std::size_t length = arrayLength(value);

    if (length != 3 && length != 4) {
        ctx.error("Expected two or three arguments.");
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
    
    ParseResult collatorParseResult;
    if (length == 4) {
        if (lhsType != type::String && rhsType != type::String) {
            ctx.error("Cannot use collator to compare non-string types.");
            return ParseResult();
        }
        collatorParseResult = ctx.parse(arrayMember(value, 3), 3, {type::Collator});
        if (!collatorParseResult) return ParseResult();
    }

    return ParseResult(std::make_unique<Equals>(std::move(*lhs), std::move(*rhs), std::move(collatorParseResult), negate));
}

} // namespace expression
} // namespace style
} // namespace mbgl
