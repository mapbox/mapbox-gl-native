#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/assertion.hpp>
#include <mbgl/style/expression/coercion.hpp>
#include <mbgl/style/expression/equals.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/util/ignore.hpp>

namespace mbgl {
namespace style {
namespace expression {
namespace dsl {

template <class... Args>
static std::vector<std::unique_ptr<Expression>> vec(Args... args) {
    std::vector<std::unique_ptr<Expression>> result;
    util::ignore({ (result.push_back(std::move(args)), 0)... });
    return result;
}

template <class... Args>
static std::unique_ptr<Expression> compound(const char* op, Args... args) {
    ParsingContext ctx;
    ParseResult result =  createCompoundExpression(op, vec(std::move(args)...), ctx);
    assert(result);
    return std::move(*result);
}

std::unique_ptr<Expression> literal(const char* value) {
    return literal(std::string(value));
}

std::unique_ptr<Expression> literal(Value value) {
    return std::make_unique<Literal>(value);
}

std::unique_ptr<Expression> number(std::unique_ptr<Expression> value) {
    return std::make_unique<Assertion>(type::Number, vec(std::move(value)));
}

std::unique_ptr<Expression> string(std::unique_ptr<Expression> value) {
    return std::make_unique<Assertion>(type::String, vec(std::move(value)));
}

std::unique_ptr<Expression> toColor(const char* value) {
    return toColor(literal(value));
}

std::unique_ptr<Expression> toColor(std::unique_ptr<Expression> value) {
    return std::make_unique<Coercion>(type::Color, vec(std::move(value)));
}

std::unique_ptr<Expression> get(const char* value) {
    return get(literal(value));
}

std::unique_ptr<Expression> get(std::unique_ptr<Expression> property) {
    return compound("get", std::move(property));
}

std::unique_ptr<Expression> id() {
    return compound("id");
}

std::unique_ptr<Expression> zoom() {
    return compound("zoom");
}

std::unique_ptr<Expression> eq(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return std::make_unique<Equals>(std::move(lhs), std::move(rhs), false);
}

std::unique_ptr<Expression> ne(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return std::make_unique<Equals>(std::move(lhs), std::move(rhs), true);
}

std::unique_ptr<Expression> gt(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return compound(">", std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expression> lt(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return compound("<", std::move(lhs), std::move(rhs));
}

Interpolator linear() {
    return ExponentialInterpolator(1.0);
}

Interpolator exponential(double base) {
    return ExponentialInterpolator(base);
}

Interpolator cubicBezier(double x1, double y1, double x2, double y2) {
    return CubicBezierInterpolator(x1, y1, x2, y2);
}

std::unique_ptr<Expression> interpolate(Interpolator interpolator,
                                        std::unique_ptr<Expression> input,
                                        double input1, std::unique_ptr<Expression> output1) {
    type::Type type = output1->getType();
    std::map<double, std::unique_ptr<Expression>> stops;
    stops[input1] = std::move(output1);
    ParsingContext ctx;
    ParseResult result = createInterpolate(type, interpolator, std::move(input), std::move(stops), ctx);
    assert(result);
    return std::move(*result);
}

std::unique_ptr<Expression> interpolate(Interpolator interpolator,
                                        std::unique_ptr<Expression> input,
                                        double input1, std::unique_ptr<Expression> output1,
                                        double input2, std::unique_ptr<Expression> output2) {
    type::Type type = output1->getType();
    std::map<double, std::unique_ptr<Expression>> stops;
    stops[input1] = std::move(output1);
    stops[input2] = std::move(output2);
    ParsingContext ctx;
    ParseResult result = createInterpolate(type, interpolator, std::move(input), std::move(stops), ctx);
    assert(result);
    return std::move(*result);
}

std::unique_ptr<Expression> interpolate(Interpolator interpolator,
                                        std::unique_ptr<Expression> input,
                                        double input1, std::unique_ptr<Expression> output1,
                                        double input2, std::unique_ptr<Expression> output2,
                                        double input3, std::unique_ptr<Expression> output3) {
    type::Type type = output1->getType();
    std::map<double, std::unique_ptr<Expression>> stops;
    stops[input1] = std::move(output1);
    stops[input2] = std::move(output2);
    stops[input3] = std::move(output3);
    ParsingContext ctx;
    ParseResult result = createInterpolate(type, interpolator, std::move(input), std::move(stops), ctx);
    assert(result);
    return std::move(*result);
}

} // namespace dsl
} // namespace expression
} // namespace style
} // namespace mbgl
