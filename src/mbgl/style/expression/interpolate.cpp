#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

using namespace mbgl::style::conversion;

template <typename T>
class InterpolateImpl : public Interpolate {
public:
    InterpolateImpl(type::Type type_,
          Interpolator interpolator_,
          std::unique_ptr<Expression> input_,
          std::map<double, std::unique_ptr<Expression>> stops_
    ) : Interpolate(std::move(type_), std::move(interpolator_), std::move(input_), std::move(stops_))
    {
        static_assert(util::Interpolatable<T>::value, "Interpolate expression requires an interpolatable value type.");
    }

    EvaluationResult evaluate(const EvaluationContext& params) const override {
        const EvaluationResult evaluatedInput = input->evaluate(params);
        if (!evaluatedInput) {
            return evaluatedInput.error();
        }

        float x = *fromExpressionValue<float>(*evaluatedInput);
        if (std::isnan(x)) {
            return EvaluationError { "Input is not a number." };
        }

        if (stops.empty()) {
            return EvaluationError { "No stops in exponential curve." };
        }

        auto it = stops.upper_bound(x);
        if (it == stops.end()) {
            return stops.rbegin()->second->evaluate(params);
        } else if (it == stops.begin()) {
            return stops.begin()->second->evaluate(params);
        } else {
            float t = interpolationFactor({ std::prev(it)->first, it->first }, x);

            if (t == 0.0f) {
                return std::prev(it)->second->evaluate(params);
            }
            if (t == 1.0f) {
                return it->second->evaluate(params);
            }

            EvaluationResult lower = std::prev(it)->second->evaluate(params);
            if (!lower) {
                return lower.error();
            }
            EvaluationResult upper = it->second->evaluate(params);
            if (!upper) {
                return upper.error();
            }

            if (!lower->is<T>()) {
                return EvaluationError {
                    "Expected value to be of type " + toString(valueTypeToExpressionType<T>()) +
                    ", but found " + toString(typeOf(*lower)) + " instead."
                };
            }

            if (!upper->is<T>()) {
                return EvaluationError {
                    "Expected value to be of type " + toString(valueTypeToExpressionType<T>()) +
                    ", but found " + toString(typeOf(*upper)) + " instead."
                };
            }
            return util::interpolate(lower->get<T>(), upper->get<T>(), t);
        }
    }
};

ParseResult parseInterpolate(const Convertible& value, ParsingContext& ctx) {
    assert(isArray(value));

    auto length = arrayLength(value);

    if (length < 2) {
        ctx.error("Expected an interpolation type expression.");
        return ParseResult();
    }

    const Convertible& interp = arrayMember(value, 1);
    if (!isArray(interp) || arrayLength(interp) == 0) {
        ctx.error("Expected an interpolation type expression.");
        return ParseResult();
    }

    optional<Interpolator> interpolator;
    
    const optional<std::string> interpName = toString(arrayMember(interp, 0));
    if (interpName && *interpName == "linear") {
        interpolator = {ExponentialInterpolator(1.0)};
    } else if (interpName && *interpName == "exponential") {
        optional<double> base;
        if (arrayLength(interp) == 2) {
            base = toDouble(arrayMember(interp, 1));
        }
        if (!base) {
            ctx.error("Exponential interpolation requires a numeric base.", 1, 1);
            return ParseResult();
        }
        interpolator = {ExponentialInterpolator(*base)};
    } else if (interpName && *interpName == "cubic-bezier") {
        optional<double> x1;
        optional<double> y1;
        optional<double> x2;
        optional<double> y2;
        if (arrayLength(interp) == 5) {
            x1 = toDouble(arrayMember(interp, 1));
            y1 = toDouble(arrayMember(interp, 2));
            x2 = toDouble(arrayMember(interp, 3));
            y2 = toDouble(arrayMember(interp, 4));
        }
        if (
            !x1 || !y1 || !x2 || !y2 ||
            *x1 < 0 || *x1 > 1 ||
            *y1 < 0 || *y1 > 1 ||
            *x2 < 0 || *x2 > 1 ||
            *y2 < 0 || *y2 > 1
        ) {
            ctx.error("Cubic bezier interpolation requires four numeric arguments with values between 0 and 1.", 1);
            return ParseResult();
            
        }
        interpolator = {CubicBezierInterpolator(*x1, *y1, *x2, *y2)};
    }
    
    if (!interpolator) {
        ctx.error("Unknown interpolation type " + (interpName ? *interpName : ""), 1, 0);
        return ParseResult();
    }
    
    std::size_t minArgs = 4;
    if (length - 1 < minArgs) {
        ctx.error("Expected at least 4 arguments, but found only " + util::toString(length - 1) + ".");
        return ParseResult();
    }
    
    // [interpolation, interp_type, input, 2 * (n pairs)...]
    if ((length - 1) % 2 != 0) {
        ctx.error("Expected an even number of arguments.");
        return ParseResult();
    }
    
    ParseResult input = ctx.parse(arrayMember(value, 2), 2, {type::Number});
    if (!input) {
        return input;
    }

    std::map<double, std::unique_ptr<Expression>> stops;
    optional<type::Type> outputType;
    if (ctx.getExpected() && *ctx.getExpected() != type::Value) {
        outputType = ctx.getExpected();
    }
    
    double previous = - std::numeric_limits<double>::infinity();
    
    for (std::size_t i = 3; i + 1 < length; i += 2) {
        const optional<mbgl::Value> labelValue = toValue(arrayMember(value, i));
        optional<double> label;
        optional<std::string> labelError;
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
            ctx.error(labelError ? *labelError :
                R"(Input/output pairs for "interpolate" expressions must be defined using literal numeric values (not computed expressions) for the input values.)",
                i);
            return ParseResult();
        }
        
        if (*label <= previous) {
            ctx.error(
                R"(Input/output pairs for "interpolate" expressions must be arranged with input values in strictly ascending order.)",
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

    return createInterpolate(*outputType,
                             *interpolator,
                             std::move(*input),
                             std::move(stops),
                             ctx);
}

ParseResult createInterpolate(type::Type type,
                              Interpolator interpolator,
                              std::unique_ptr<Expression> input,
                              std::map<double, std::unique_ptr<Expression>> stops,
                              ParsingContext& ctx) {
    return type.match(
        [&](const type::NumberType&) -> ParseResult {
            return ParseResult(std::make_unique<InterpolateImpl<double>>(
                type, interpolator, std::move(input), std::move(stops)
            ));
        },
        [&](const type::ColorType&) -> ParseResult {
            return ParseResult(std::make_unique<InterpolateImpl<Color>>(
                type, interpolator, std::move(input), std::move(stops)
            ));
        },
        [&](const type::Array& arrayType) -> ParseResult {
            if (arrayType.itemType != type::Number || !arrayType.N) {
                ctx.error("Type " + toString(type) + " is not interpolatable.");
                return ParseResult();
            }
            return ParseResult(std::make_unique<InterpolateImpl<std::vector<Value>>>(
                type, interpolator, std::move(input), std::move(stops)
            ));
        },
        [&](const auto&) {
            ctx.error("Type " + toString(type) + " is not interpolatable.");
            return ParseResult();
        }
    );
}

Interpolate::Interpolate(const type::Type& type_,
                         Interpolator interpolator_,
                         std::unique_ptr<Expression> input_,
                         std::map<double, std::unique_ptr<Expression>> stops_)
  : Expression(Kind::Interpolate, type_),
    interpolator(std::move(interpolator_)),
    input(std::move(input_)),
    stops(std::move(stops_)) {
    assert(input->getType() == type::Number);
}

std::vector<optional<Value>> Interpolate::possibleOutputs() const {
    std::vector<optional<Value>> result;
    for (const auto& stop : stops) {
        for (auto& output : stop.second->possibleOutputs()) {
            result.push_back(std::move(output));
        }
    }
    return result;
}

mbgl::Value Interpolate::serialize() const {
    std::vector<mbgl::Value> serialized;
    serialized.emplace_back(getOperator());
    
    interpolator.match(
        [&](const ExponentialInterpolator& exponential) {
            if (exponential.base == 1) {
                serialized.emplace_back(std::vector<mbgl::Value>{{ std::string("linear") }});
            } else {
                serialized.emplace_back(std::vector<mbgl::Value>{{ std::string("exponential"), exponential.base }});
            }
        },
        [&](const CubicBezierInterpolator& cubicBezier) {
            static const std::string cubicBezierTag("cubic-bezier");
            auto p1 = cubicBezier.ub.getP1();
            auto p2 = cubicBezier.ub.getP2();
            serialized.emplace_back(std::vector<mbgl::Value>{{ cubicBezierTag, p1.first, p1.second, p2.first, p2.second }});
        }
    );
    serialized.emplace_back(input->serialize());
    for (auto& entry : stops) {
        serialized.emplace_back(entry.first);
        serialized.emplace_back(entry.second->serialize());
    };
    return serialized;
}

} // namespace expression
} // namespace style
} // namespace mbgl
