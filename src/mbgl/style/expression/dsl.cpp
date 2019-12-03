#include <mbgl/style/expression/assertion.hpp>
#include <mbgl/style/expression/coercion.hpp>
#include <mbgl/style/expression/comparison.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/dsl_impl.hpp>
#include <mbgl/style/expression/error.hpp>
#include <mbgl/style/expression/format_expression.hpp>
#include <mbgl/style/expression/image_expression.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/step.hpp>

#include <mapbox/geojsonvt.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <rapidjson/document.h>

namespace mbgl {
namespace style {
namespace expression {
namespace dsl {

std::unique_ptr<Expression> compound(const char* op, std::vector<std::unique_ptr<Expression>> args) {
    ParsingContext ctx;
    ParseResult result =  createCompoundExpression(op, std::move(args), ctx);
    assert(result);
    return std::move(*result);
}
    
std::unique_ptr<Expression> createExpression(const char* expr) {
    using JSValue = rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>;
    rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator> document;
    document.Parse<0>(expr);
    if (document.HasParseError()) return nullptr;

    const JSValue* expression = &document;
    expression::ParsingContext ctx;
    expression::ParseResult parsed =
    ctx.parseExpression(mbgl::style::conversion::Convertible(expression));
    return parsed ? std::move(*parsed) : nullptr;
}

std::unique_ptr<Expression> createExpression(const mbgl::style::conversion::Convertible& expr) {
    expression::ParsingContext ctx;
    expression::ParseResult parsed = ctx.parseExpression(expr);
    return parsed ? std::move(*parsed) : nullptr;
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
        values.emplace_back(i);
    }
    return literal(values);
}

std::unique_ptr<Expression> literal(std::initializer_list<const char *> value) {
    std::vector<Value> values;
    for (auto i : value) {
        values.emplace_back(std::string(i));
    }
    return literal(values);
}

std::unique_ptr<Expression> assertion(type::Type type,
                                      std::unique_ptr<Expression> value,
                                      std::unique_ptr<Expression> def) {
    std::vector<std::unique_ptr<Expression>> v  = vec(std::move(value));
    if (def) {
        v.push_back(std::move(def));
    }
    return std::make_unique<Assertion>(type, std::move(v));
}

std::unique_ptr<Expression> number(std::unique_ptr<Expression> value,
                                   std::unique_ptr<Expression> def) {
    return assertion(type::Number, std::move(value), std::move(def));
}

std::unique_ptr<Expression> string(std::unique_ptr<Expression> value,
                                   std::unique_ptr<Expression> def) {
    return assertion(type::String, std::move(value), std::move(def));
}

std::unique_ptr<Expression> boolean(std::unique_ptr<Expression> value,
                                    std::unique_ptr<Expression> def) {
    return assertion(type::Boolean, std::move(value), std::move(def));
}

std::unique_ptr<Expression> coercion(type::Type type,
                                     std::unique_ptr<Expression> value,
                                     std::unique_ptr<Expression> def) {
    std::vector<std::unique_ptr<Expression>> v = vec(std::move(value));
    if (def) {
        v.push_back(std::move(def));
    }
    return std::make_unique<Coercion>(type, std::move(v));
}

std::unique_ptr<Expression> toColor(std::unique_ptr<Expression> value,
                                    std::unique_ptr<Expression> def) {
    return coercion(type::Color, std::move(value), std::move(def));
}

std::unique_ptr<Expression> toString(std::unique_ptr<Expression> value,
                                     std::unique_ptr<Expression> def) {
    return coercion(type::String, std::move(value), std::move(def));
}

std::unique_ptr<Expression> toFormatted(std::unique_ptr<Expression> value,
                                        std::unique_ptr<Expression> def) {
    return coercion(type::Formatted, std::move(value), std::move(def));
}

std::unique_ptr<Expression> toImage(std::unique_ptr<Expression> value, std::unique_ptr<Expression> def) {
    return coercion(type::Image, std::move(value), std::move(def));
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
    return std::make_unique<BasicComparison>("==", std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expression> ne(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return std::make_unique<BasicComparison>("!=", std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expression> gt(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return std::make_unique<BasicComparison>(">", std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expression> lt(std::unique_ptr<Expression> lhs,
                               std::unique_ptr<Expression> rhs) {
    return std::make_unique<BasicComparison>("<", std::move(lhs), std::move(rhs));
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

std::unique_ptr<Expression> format(const char* value) {
    return std::make_unique<Literal>(Formatted(value));
}

std::unique_ptr<Expression> format(std::unique_ptr<Expression> input) {
    std::vector<FormatExpressionSection> sections{FormatExpressionSection(std::move(input))};
    return std::make_unique<FormatExpression>(std::move(sections));
}

std::unique_ptr<Expression> image(const char* value) {
    return std::make_unique<Literal>(Image(value));
}

std::unique_ptr<Expression> image(std::unique_ptr<Expression> expression) {
    return std::make_unique<ImageExpression>(std::move(expression));
}

} // namespace dsl
} // namespace expression
} // namespace style
} // namespace mbgl
