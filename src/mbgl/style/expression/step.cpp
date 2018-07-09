#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/get_covering_stops.hpp>
#include <mbgl/util/string.hpp>

#include <cmath>

namespace mbgl {
namespace style {
namespace expression {

Step::Step(const type::Type& type_,
           std::unique_ptr<Expression> input_,
           std::map<double, std::unique_ptr<Expression>> stops_)
  : Expression(Kind::Step, type_),
    input(std::move(input_)),
    stops(std::move(stops_))
{
    assert(input->getType() == type::Number);
}

EvaluationResult Step::evaluate(const EvaluationContext& params) const {
    const EvaluationResult evaluatedInput = input->evaluate(params);
    if (!evaluatedInput) {
        return evaluatedInput.error();
    }

    float x = *fromExpressionValue<float>(*evaluatedInput);
    if (std::isnan(x)) {
        return EvaluationError { "Input is not a number." };
    }

    if (stops.empty()) {
        return EvaluationError { "No stops in step curve." };
    }

    auto it = stops.upper_bound(x);
    if (it == stops.end()) {
        return stops.rbegin()->second->evaluate(params);
    } else if (it == stops.begin()) {
        return stops.begin()->second->evaluate(params);
    } else {
        return std::prev(it)->second->evaluate(params);
    }
}

void Step::eachChild(const std::function<void(const Expression&)>& visit) const {
    visit(*input);
    for (auto it = stops.begin(); it != stops.end(); it++) {
        visit(*it->second);
    }
}

void Step::eachStop(const std::function<void(double, const Expression&)>& visit) const {
    for (const auto &stop : stops) {
        visit(stop.first, *stop.second);
    }
}

bool Step::operator==(const Expression& e) const {
    if (e.getKind() == Kind::Step) {
        auto rhs = static_cast<const Step*>(&e);
        return *input == *(rhs->input) && Expression::childrenEqual(stops, rhs->stops);
    }
    return false;
}

std::vector<optional<Value>> Step::possibleOutputs() const {
    std::vector<optional<Value>> result;
    for (const auto& stop : stops) {
        for (auto& output : stop.second->possibleOutputs()) {
            result.push_back(std::move(output));
        }
    }
    return result;
}

Range<float> Step::getCoveringStops(const double lower, const double upper) const {
    return ::mbgl::style::expression::getCoveringStops(stops, lower, upper);
}


ParseResult Step::parse(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));

    auto length = arrayLength(value);

    if (length - 1 < 4) {
        ctx.error("Expected at least 4 arguments, but found only " + util::toString(length - 1) + ".");
        return ParseResult();
    }
    
    // [step, input, firstOutput_value, 2 * (n pairs)...]
    if ((length - 1) % 2 != 0) {
        ctx.error("Expected an even number of arguments.");
        return ParseResult();
    }
    
    ParseResult input = ctx.parse(arrayMember(value, 1), 1, {type::Number});
    if (!input) {
        return input;
    }
    
    std::map<double, std::unique_ptr<Expression>> stops;
    optional<type::Type> outputType;
    if (ctx.getExpected() && *ctx.getExpected() != type::Value) {
        outputType = ctx.getExpected();
    }
    
    double previous = - std::numeric_limits<double>::infinity();
    
    // consume the first output value, which doesn't have a corresponding input value,
    // before proceeding into the "stops" loop below.
    auto firstOutput = ctx.parse(arrayMember(value, 2), 2, outputType);
    if (!firstOutput) {
        return ParseResult();
    }
    if (!outputType) {
        outputType = (*firstOutput)->getType();
    }
    stops.emplace(-std::numeric_limits<double>::infinity(), std::move(*firstOutput));
    
    
    for (std::size_t i = 3; i + 1 < length; i += 2) {
        const optional<mbgl::Value> labelValue = toValue(arrayMember(value, i));
        optional<double> label;
        if (labelValue) {
            labelValue->match(
                [&](uint64_t n) {
                    if (n > std::numeric_limits<double>::max()) {
                        label = optional<double>{std::numeric_limits<double>::infinity()};
                    } else {
                        label = optional<double>{static_cast<double>(n)};
                    }
                },
                [&](int64_t n) {
                    if (n > std::numeric_limits<double>::max()) {
                        label = optional<double>{std::numeric_limits<double>::infinity()};
                    } else {
                        label = optional<double>{static_cast<double>(n)};
                    }
                },
                [&](double n) {
                    if (n > std::numeric_limits<double>::max()) {
                        label = optional<double>{std::numeric_limits<double>::infinity()};
                    } else {
                        label = optional<double>{n};
                    }
                },
                [&](const auto&) {}
            );
        }
        if (!label) {
            ctx.error(R"(Input/output pairs for "step" expressions must be defined using literal numeric values (not computed expressions) for the input values.)", i);
            return ParseResult();
        }
        
        if (*label <= previous) {
            ctx.error(
                R"(Input/output pairs for "step" expressions must be arranged with input values in strictly ascending order.)",
                i
            );
            return ParseResult();
        }
        previous = *label;
        
        auto output = ctx.parse(arrayMember(value, i + 1), i + 1, outputType);
        if (!output) {
            return ParseResult();
        }
        if (!outputType) {
            outputType = (*output)->getType();
        }

        stops.emplace(*label, std::move(*output));
    }
    
    assert(outputType);
    
    return ParseResult(std::make_unique<Step>(*outputType, std::move(*input), std::move(stops)));
}

mbgl::Value Step::serialize() const {
    std::vector<mbgl::Value> serialized;
    serialized.emplace_back(getOperator());
    serialized.emplace_back(input->serialize());
    for (auto& entry : stops) {
        if (entry.first > -std::numeric_limits<double>::infinity()) {
            serialized.emplace_back(entry.first);
        }
        serialized.emplace_back(entry.second->serialize());
    }
    return serialized;
}

} // namespace expression
} // namespace style
} // namespace mbgl

