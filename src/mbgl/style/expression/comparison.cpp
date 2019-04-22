#include <mbgl/style/expression/collator.hpp>
#include <mbgl/style/expression/comparison.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <utility>

namespace mbgl {
namespace style {
namespace expression {

static bool isComparableType(const std::string& op, const type::Type& type) {
    if (op == "==" || op == "!=") {
        return type == type::String ||
            type == type::Number ||
            type == type::Boolean ||
            type == type::Null ||
            type == type::Value;
    } else {
        return type == type::String ||
            type == type::Number ||
            type == type::Value;
    }
}

bool eq(Value a, Value b) { return a == b; }
bool neq(Value a, Value b) { return a != b; }
bool lt(Value lhs, Value rhs) {
    return lhs.match(
        [&](const std::string& a) { return a < rhs.get<std::string>(); },
        [&](double a) { return a < rhs.get<double>(); },
        [&](const auto&) { assert(false); return false; }
    );
}
bool gt(Value lhs, Value rhs) {
    return lhs.match(
        [&](const std::string& a) { return a > rhs.get<std::string>(); },
        [&](double a) { return a > rhs.get<double>(); },
        [&](const auto&) { assert(false); return false; }
    );
}
bool lteq(Value lhs, Value rhs) {
    return lhs.match(
        [&](const std::string& a) { return a <= rhs.get<std::string>(); },
        [&](double a) { return a <= rhs.get<double>(); },
        [&](const auto&) { assert(false); return false; }
    );
}
bool gteq(Value lhs, Value rhs) {
    return lhs.match(
        [&](const std::string& a) { return a >= rhs.get<std::string>(); },
        [&](double a) { return a >= rhs.get<double>(); },
        [&](const auto&) { assert(false); return false; }
    );
}

bool eqCollate(std::string a, std::string b, Collator c) { return c.compare(a, b) == 0; }
bool neqCollate(std::string a, std::string b, Collator c) { return !eqCollate(a, b, c); }
bool ltCollate(std::string a, std::string b, Collator c) { return c.compare(a, b) < 0; }
bool gtCollate(std::string a, std::string b, Collator c) { return c.compare(a, b) > 0; }
bool lteqCollate(std::string a, std::string b, Collator c) { return c.compare(a, b) <= 0; }
bool gteqCollate(std::string a, std::string b, Collator c) { return c.compare(a, b) >= 0; }

static BasicComparison::CompareFunctionType getBasicCompareFunction(const std::string& op) {
    if (op == "==") return eq;
    else if (op == "!=") return neq;
    else if (op == ">") return gt;
    else if (op == "<") return lt;
    else if (op == ">=") return gteq;
    else if (op == "<=") return lteq;
    assert(false);
    return nullptr;
}

static CollatorComparison::CompareFunctionType getCollatorComparisonFunction(const std::string& op) {
    if (op == "==") return eqCollate;
    else if (op == "!=") return neqCollate;
    else if (op == ">") return gtCollate;
    else if (op == "<") return ltCollate;
    else if (op == ">=") return gteqCollate;
    else if (op == "<=") return lteqCollate;
    assert(false);
    return nullptr;

}


BasicComparison::BasicComparison(
        std::string op_,
        std::unique_ptr<Expression> lhs_,
        std::unique_ptr<Expression> rhs_)
    : Expression(Kind::Comparison, type::Boolean),
      op(std::move(op_)),
      compare(getBasicCompareFunction(op)),
      lhs(std::move(lhs_)),
      rhs(std::move(rhs_)) {
    assert(isComparableType(op, lhs->getType()) && isComparableType(op, rhs->getType()));
    assert(lhs->getType() == rhs->getType() || lhs->getType() == type::Value || rhs->getType() == type::Value);
    
    needsRuntimeTypeCheck = (op != "==" && op != "!=") &&
        (lhs->getType() == type::Value || rhs->getType() == type::Value);
}

EvaluationResult BasicComparison::evaluate(const EvaluationContext& params) const {
    EvaluationResult lhsResult = lhs->evaluate(params);
    if (!lhsResult) return lhsResult;

    EvaluationResult rhsResult = rhs->evaluate(params);
    if (!rhsResult) return rhsResult;
    
    if (needsRuntimeTypeCheck) {
        type::Type lhsType = typeOf(*lhsResult);
        type::Type rhsType = typeOf(*rhsResult);
        // check that type is string or number, and equal
        if (lhsType != rhsType || !(lhsType == type::String || lhsType == type::Number)) {
            return EvaluationError {
                R"(Expected arguments for ")" + op + R"(")" +
                " to be (string, string) or (number, number), but found (" + toString(lhsType) + ", " +
                toString(rhsType) + ") instead."
            };
        }
    }
    
    return compare(*lhsResult, *rhsResult);
}

void BasicComparison::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*lhs);
    visit(*rhs);
}

std::string BasicComparison::getOperator() const { return op; }

bool BasicComparison::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Comparison) {
        auto comp = static_cast<const BasicComparison*>(&e);
        return comp->op == op &&
            *comp->lhs == *lhs &&
            *comp->rhs == *rhs;
    }
    return false;
}

std::vector<optional<Value>> BasicComparison::possibleOutputs() const {
    return {{ true }, { false }};
}

CollatorComparison::CollatorComparison(
        std::string op_,
        std::unique_ptr<Expression> lhs_,
        std::unique_ptr<Expression> rhs_,
        std::unique_ptr<Expression> collator_)
    : Expression(Kind::Comparison, type::Boolean),
      op(std::move(op_)),
      compare(getCollatorComparisonFunction(op)),
      lhs(std::move(lhs_)),
      rhs(std::move(rhs_)),
      collator(std::move(collator_)) {
    assert(isComparableType(op, lhs->getType()) && isComparableType(op, rhs->getType()));
    assert(lhs->getType() == rhs->getType() || lhs->getType() == type::Value || rhs->getType() == type::Value);

    needsRuntimeTypeCheck = (op == "==" || op == "!=") &&
        (lhs->getType() == type::Value || rhs->getType() == type::Value);
}

EvaluationResult CollatorComparison::evaluate(const EvaluationContext& params) const {
    EvaluationResult lhsResult = lhs->evaluate(params);
    if (!lhsResult) return lhsResult;

    EvaluationResult rhsResult = rhs->evaluate(params);
    if (!rhsResult) return lhsResult;
    
    if (needsRuntimeTypeCheck) {
        if (typeOf(*lhsResult) != type::String || typeOf(*rhsResult) != type::String) {
            return getBasicCompareFunction(op)(*lhsResult, *rhsResult);
        }
    }
    
    auto collatorResult = collator->evaluate(params);
    if (!collatorResult) return collatorResult;
    
    const Collator& c = collatorResult->get<Collator>();
    return compare(lhsResult->get<std::string>(), rhsResult->get<std::string>(), c);
}

void CollatorComparison::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*lhs);
    visit(*rhs);
    visit(*collator);
}

std::string CollatorComparison::getOperator() const { return op; }

bool CollatorComparison::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Comparison) {
        auto comp = static_cast<const CollatorComparison*>(&e);
        return comp->op == op &&
            *comp->collator == *collator &&
            *comp->lhs == *lhs &&
            *comp->rhs == *rhs;
    }
    return false;
}

std::vector<optional<Value>> CollatorComparison::possibleOutputs() const {
    return {{ true }, { false }};
}

using namespace mbgl::style::conversion;
ParseResult parseComparison(const Convertible& value, ParsingContext& ctx) {
    std::size_t length = arrayLength(value);

    if (length != 3 && length != 4) {
        ctx.error("Expected two or three arguments.");
        return ParseResult();
    }
    
    std::string op = *toString(arrayMember(value, 0));
    
    assert(getBasicCompareFunction(op));

    ParseResult lhs = ctx.parse(arrayMember(value, 1), 1, {type::Value});
    if (!lhs) return ParseResult();
    type::Type lhsType = (*lhs)->getType();
    if (!isComparableType(op, lhsType)) {
        ctx.error(R"(")" + op + R"(" comparisons are not supported for type ')" + toString(lhsType) + "'.", 1);
        return ParseResult();
    }

    ParseResult rhs = ctx.parse(arrayMember(value, 2), 2, {type::Value});
    if (!rhs) return ParseResult();
    type::Type rhsType = (*rhs)->getType();
    if (!isComparableType(op, rhsType)) {
        ctx.error(R"(")" + op + R"(" comparisons are not supported for type ')" + toString(rhsType) + "'.", 2);
        return ParseResult();
    }

    if (
        lhsType != rhsType &&
        lhsType != type::Value &&
        rhsType != type::Value
    ) {
        ctx.error("Cannot compare types '" + toString(lhsType) + "' and '" + toString(rhsType) + "'.");
        return ParseResult();
    }
    
    if (op != "==" && op != "!=") {
        // typing rules specific to less/greater than operators
        if (lhsType == type::Value && rhsType != type::Value) {
            // (value, T)
            lhs = dsl::assertion(rhsType, std::move(*lhs));
        } else if (lhsType != type::Value && rhsType == type::Value) {
            // (T, value)
            rhs = dsl::assertion(lhsType, std::move(*rhs));
        }
    }
    
    if (length == 4) {
        if (
            lhsType != type::String &&
            rhsType != type::String &&
            lhsType != type::Value &&
            rhsType != type::Value
        ) {
            ctx.error("Cannot use collator to compare non-string types.");
            return ParseResult();
        }
        ParseResult collatorParseResult = ctx.parse(arrayMember(value, 3), 3, {type::Collator});
        if (!collatorParseResult) return ParseResult();
        return ParseResult(std::make_unique<CollatorComparison>(op, std::move(*lhs), std::move(*rhs), std::move(*collatorParseResult)));
    }
    
    return ParseResult(std::make_unique<BasicComparison>(op, std::move(*lhs), std::move(*rhs)));
}

} // namespace expression
} // namespace style
} // namespace mbgl
