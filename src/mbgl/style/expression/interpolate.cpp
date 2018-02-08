#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/util/string.hpp>

namespace mbgl {
namespace style {
namespace expression {

using Interpolator = variant<ExponentialInterpolator,
                             CubicBezierInterpolator>;

using namespace mbgl::style::conversion;

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
                        label = {std::numeric_limits<double>::infinity()};
                    } else {
                        label = {static_cast<double>(n)};
                    }
                },
                [&](int64_t n) {
                    if (n > std::numeric_limits<double>::max()) {
                        label = {std::numeric_limits<double>::infinity()};
                    } else {
                        label = {static_cast<double>(n)};
                    }
                },
                [&](double n) {
                    if (n > std::numeric_limits<double>::max()) {
                        label = {std::numeric_limits<double>::infinity()};
                    } else {
                        label = {static_cast<double>(n)};
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
    
    if (
        *outputType != type::Number &&
        *outputType != type::Color &&
        !(
            outputType->is<type::Array>() &&
            outputType->get<type::Array>().itemType == type::Number &&
            outputType->get<type::Array>().N
        )
    )
    {
        ctx.error("Type " + toString(*outputType) + " is not interpolatable.");
        return ParseResult();
    }
    
    return outputType->match(
        [&](const type::NumberType&) -> ParseResult {
            return interpolator->match([&](const auto& interpolator_) {
                return ParseResult(std::make_unique<Interpolate<double>>(
                    *outputType, interpolator_, std::move(*input), std::move(stops)
                ));
            });
        },
        [&](const type::ColorType&) -> ParseResult {
            return interpolator->match([&](const auto& interpolator_) {
                return ParseResult(std::make_unique<Interpolate<Color>>(
                    *outputType, interpolator_, std::move(*input), std::move(stops)
                ));
            });
        },
        [&](const type::Array& arrayType) -> ParseResult {
            return interpolator->match(
                [&](const auto& continuousInterpolator) {
                    if (arrayType.itemType != type::Number || !arrayType.N) {
                        assert(false); // interpolability already checked above.
                        return ParseResult();
                    }
                    return ParseResult(std::make_unique<Interpolate<std::vector<Value>>>(
                        *outputType, continuousInterpolator, std::move(*input), std::move(stops)
                    ));
                }
            );
        },
        [&](const auto&) {
            // unreachable: Null, Boolean, String, Object, Value output types
            // are not interpolatable, and interpolability was already checked above
            assert(false);
            return ParseResult();
        }
    );
}

std::vector<optional<Value>> InterpolateBase::possibleOutputs() const {
    std::vector<optional<Value>> result;
    for (const auto& stop : stops) {
        for (auto& output : stop.second->possibleOutputs()) {
            result.push_back(std::move(output));
        }
    }
    return result;
}

template <typename T>
mbgl::Value Interpolate<T>::serialize() const {
    std::vector<mbgl::Value> serialized;
    serialized.emplace_back(getOperator());
    
    interpolator.match(
        [&](const ExponentialInterpolator& exponential) {
            serialized.emplace_back(std::vector<mbgl::Value>{{ std::string("exponential"), exponential.base }});
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
