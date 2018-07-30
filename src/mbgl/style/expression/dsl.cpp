#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/error.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/assertion.hpp>
#include <mbgl/style/expression/coercion.hpp>
#include <mbgl/style/expression/equals.hpp>
#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/compound_expression.hpp>

namespace mbgl {
namespace style {
namespace expression {
namespace dsl {

static std::unique_ptr<Expression> compound(const char* op, std::vector<std::unique_ptr<Expression>> args) {
    ParsingContext ctx;
    ParseResult result =  createCompoundExpression(op, std::move(args), ctx);
    assert(result);
    return std::move(*result);
}

template <class... Args>
static std::unique_ptr<Expression> compound(const char* op, Args... args) {
    return compound(op, vec(std::move(args)...));
}

std::unique_ptr<Expression> error(std::string message) {
    return std::make_unique<Error>(std::move(message));
}

std::unique_ptr<Expression> literal(const char* value) {
    return literal(std::string(value));
}

std::unique_ptr<Expression> literal(Value value) {
    return std::make_unique<Literal>(value);
}

std::unique_ptr<Expression> literal(std::initializer_list<double> value) {
    std::vector<Value> values;
    for (auto i : value) {
        values.push_back(i);
    }
    return literal(values);
}

std::unique_ptr<Expression> literal(std::initializer_list<const char *> value) {
    std::vector<Value> values;
    for (auto i : value) {
        values.push_back(std::string(i));
    }
    return literal(values);
}

std::unique_ptr<Expression> number(std::unique_ptr<Expression> value) {
    return std::make_unique<Assertion>(type::Number, vec(std::move(value)));
}

std::unique_ptr<Expression> string(std::unique_ptr<Expression> value) {
    return std::make_unique<Assertion>(type::String, vec(std::move(value)));
}

std::unique_ptr<Expression> boolean(std::unique_ptr<Expression> value) {
    return std::make_unique<Assertion>(type::Boolean, vec(std::move(value)));
}

std::unique_ptr<Expression> toColor(std::unique_ptr<Expression> value) {
    return std::make_unique<Coercion>(type::Color, vec(std::move(value)));
}

std::unique_ptr<Expression> toString(std::unique_ptr<Expression> value) {
    return compound("to-string", std::move(value));
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
    return std::make_unique<Equals>(std::move(lhs), std::move(rhs), nullopt, false);
}

std::unique_ptr<Expression> ne(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return std::make_unique<Equals>(std::move(lhs), std::move(rhs), nullopt, true);
}

std::unique_ptr<Expression> gt(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return compound(">", std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expression> lt(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return compound("<", std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expression> step(std::unique_ptr<Expression> input,
                                 std::unique_ptr<Expression> output0,
                                 double input1, std::unique_ptr<Expression> output1) {
    type::Type type = output0->getType();
    std::map<double, std::unique_ptr<Expression>> stops;
    stops[-std::numeric_limits<double>::infinity()] = std::move(output0);
    stops[input1] = std::move(output1);
    return std::make_unique<Step>(type, std::move(input), std::move(stops));
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

std::unique_ptr<Expression> concat(std::vector<std::unique_ptr<Expression>> inputs) {
    return compound("concat", std::move(inputs));
}

} // namespace dsl
} // namespace expression
} // namespace style
} // namespace mbgl
