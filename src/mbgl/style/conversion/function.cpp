#include <mbgl/style/conversion/function.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/match.hpp>
#include <mbgl/style/expression/case.hpp>
#include <mbgl/style/expression/array_assertion.hpp>
#include <mbgl/util/string.hpp>

#include <cassert>

namespace mbgl {
namespace style {
namespace conversion {

using namespace expression;
using namespace expression::dsl;

const static std::string tokenReservedChars = "{}";

bool hasTokens(const std::string& source) {
    auto pos = source.begin();
    const auto end = source.end();

    while (pos != end) {
        auto brace = std::find(pos, end, '{');
        if (brace == end)
            return false;
        for (brace++; brace != end && tokenReservedChars.find(*brace) == std::string::npos; brace++);
        if (brace != end && *brace == '}') {
            return true;
        }
        pos = brace;
    }

    return false;
}

std::unique_ptr<Expression> convertTokenStringToExpression(const std::string& source) {
    std::vector<std::unique_ptr<Expression>> inputs;

    auto pos = source.begin();
    const auto end = source.end();

    while (pos != end) {
        auto brace = std::find(pos, end, '{');
        if (pos != brace) {
            inputs.push_back(literal(std::string(pos, brace)));
        }
        pos = brace;
        if (pos != end) {
            for (brace++; brace != end && tokenReservedChars.find(*brace) == std::string::npos; brace++);
            if (brace != end && *brace == '}') {
                inputs.push_back(toString(get(literal(std::string(pos + 1, brace)))));
                pos = brace + 1;
            } else {
                inputs.push_back(literal(std::string(pos, brace)));
                pos = brace;
            }
        }
    }

    switch (inputs.size()) {
    case 0:
        return literal(source);
    case 1:
        return std::move(inputs[0]);
    default:
        return concat(std::move(inputs));
    }
}

// Ad-hoc Converters for double and int64_t. We should replace float with double wholesale,
// and promote the int64_t Converter to general use (and it should check that the input is
// an integer).
template <>
struct Converter<double> {
    optional<double> operator()(const Convertible& value, Error& error) const {
        auto converted = convert<float>(value, error);
        if (!converted) {
            return {};
        }
        return *converted;
    }
};

template <>
struct Converter<int64_t> {
    optional<int64_t> operator()(const Convertible& value, Error& error) const {
        auto converted = convert<float>(value, error);
        if (!converted) {
            return {};
        }
        return *converted;
    }
};

enum class FunctionType {
    Interval,
    Exponential,
    Categorical,
    Identity,
    Invalid
};

static bool interpolatable(type::Type type) {
    return type.match(
        [&] (const type::NumberType&) {
            return true;
        },
        [&] (const type::ColorType&) {
            return true;
        },
        [&] (const type::Array& array) {
            return array.N && array.itemType == type::Number;
        },
        [&] (const auto&) {
            return false;
        }
    );
}

static optional<std::unique_ptr<Expression>> convertLiteral(type::Type type, const Convertible& value, Error& error, bool convertTokens = false) {
    return type.match(
        [&] (const type::NumberType&) -> optional<std::unique_ptr<Expression>> {
            auto result = convert<float>(value, error);
            if (!result) {
                return {};
            }
            return literal(double(*result));
        },
        [&] (const type::BooleanType&) -> optional<std::unique_ptr<Expression>> {
            auto result = convert<bool>(value, error);
            if (!result) {
                return {};
            }
            return literal(*result);
        },
        [&] (const type::StringType&) -> optional<std::unique_ptr<Expression>> {
            auto result = convert<std::string>(value, error);
            if (!result) {
                return {};
            }
            return convertTokens ? convertTokenStringToExpression(*result) : literal(*result);
        },
        [&] (const type::ColorType&) -> optional<std::unique_ptr<Expression>> {
            auto result = convert<Color>(value, error);
            if (!result) {
                return {};
            }
            return literal(*result);
        },
        [&] (const type::Array& array) -> optional<std::unique_ptr<Expression>> {
            if (!isArray(value)) {
                error = { "value must be an array" };
                return {};
            }
            if (array.N && arrayLength(value) != *array.N) {
                error = { "value must be an array of length " + util::toString(*array.N) };
                return {};
            }
            return array.itemType.match(
                [&] (const type::NumberType&) -> optional<std::unique_ptr<Expression>> {
                    std::vector<expression::Value> result;
                    result.reserve(arrayLength(value));
                    for (std::size_t i = 0; i < arrayLength(value); ++i) {
                        optional<float> number = toNumber(arrayMember(value, i));
                        if (!number) {
                            error = { "value must be an array of numbers" };
                            return {};
                        }
                        result.push_back(double(*number));
                    }
                    return literal(result);
                },
                [&] (const type::StringType&) -> optional<std::unique_ptr<Expression>> {
                    std::vector<expression::Value> result;
                    result.reserve(arrayLength(value));
                    for (std::size_t i = 0; i < arrayLength(value); ++i) {
                        optional<std::string> string = toString(arrayMember(value, i));
                        if (!string) {
                            error = { "value must be an array of strings" };
                            return {};
                        }
                        result.push_back(*string);
                    }
                    return literal(result);
                },
                [&] (const auto&) -> optional<std::unique_ptr<Expression>> {
                    assert(false); // No properties use this type.
                    return {};
                }
            );
        },
        [&] (const type::NullType&) -> optional<std::unique_ptr<Expression>> {
            assert(false); // No properties use this type.
            return {};
        },
        [&] (const type::ObjectType&) -> optional<std::unique_ptr<Expression>> {
            assert(false); // No properties use this type.
            return {};
        },
        [&] (const type::ErrorType&) -> optional<std::unique_ptr<Expression>> {
            assert(false); // No properties use this type.
            return {};
        },
        [&] (const type::ValueType&) -> optional<std::unique_ptr<Expression>> {
            assert(false); // No properties use this type.
            return {};
        },
        [&] (const type::CollatorType&) -> optional<std::unique_ptr<Expression>> {
            assert(false); // No properties use this type.
            return {};
        }
    );
}

static optional<std::map<double, std::unique_ptr<Expression>>> convertStops(type::Type type,
                                                                            const Convertible& value,
                                                                            Error& error,
                                                                            bool convertTokens) {
    auto stopsValue = objectMember(value, "stops");
    if (!stopsValue) {
        error = { "function value must specify stops" };
        return {};
    }

    if (!isArray(*stopsValue)) {
        error = { "function stops must be an array" };
        return {};
    }

    if (arrayLength(*stopsValue) == 0) {
        error = { "function must have at least one stop" };
        return {};
    }

    std::map<double, std::unique_ptr<Expression>> stops;
    for (std::size_t i = 0; i < arrayLength(*stopsValue); ++i) {
        const auto& stopValue = arrayMember(*stopsValue, i);

        if (!isArray(stopValue)) {
            error = { "function stop must be an array" };
            return {};
        }

        if (arrayLength(stopValue) != 2) {
            error = { "function stop must have two elements" };
            return {};
        }

        optional<float> t = convert<float>(arrayMember(stopValue, 0), error);
        if (!t) {
            return {};
        }

        optional<std::unique_ptr<Expression>> e = convertLiteral(type, arrayMember(stopValue, 1), error, convertTokens);
        if (!e) {
            return {};
        }

        stops.emplace(*t, std::move(*e));
    }

    return { std::move(stops) };
}

template <class T>
optional<std::map<T, std::unique_ptr<Expression>>> convertBranches(type::Type type,
                                                                   const Convertible& value,
                                                                   Error& error) {
    auto stopsValue = objectMember(value, "stops");
    if (!stopsValue) {
        error = { "function value must specify stops" };
        return {};
    }

    if (!isArray(*stopsValue)) {
        error = { "function stops must be an array" };
        return {};
    }

    if (arrayLength(*stopsValue) == 0) {
        error = { "function must have at least one stop" };
        return {};
    }

    std::map<T, std::unique_ptr<Expression>> stops;
    for (std::size_t i = 0; i < arrayLength(*stopsValue); ++i) {
        const auto& stopValue = arrayMember(*stopsValue, i);

        if (!isArray(stopValue)) {
            error = { "function stop must be an array" };
            return {};
        }

        if (arrayLength(stopValue) != 2) {
            error = { "function stop must have two elements" };
            return {};
        }

        optional<T> t = convert<T>(arrayMember(stopValue, 0), error);
        if (!t) {
            return {};
        }

        optional<std::unique_ptr<Expression>> e = convertLiteral(type, arrayMember(stopValue, 1), error);
        if (!e) {
            return {};
        }

        stops.emplace(*t, std::move(*e));
    }

    return { std::move(stops) };
}

static optional<double> convertBase(const Convertible& value, Error& error) {
    auto baseValue = objectMember(value, "base");

    if (!baseValue) {
        return 1.0;
    }

    auto base = toNumber(*baseValue);
    if (!base) {
        error = { "function base must be a number" };
        return {};
    }

    return *base;
}

static std::unique_ptr<Expression> step(type::Type type, std::unique_ptr<Expression> input, std::map<double, std::unique_ptr<Expression>> stops) {
    return std::make_unique<Step>(type, std::move(input), std::move(stops));
}

static std::unique_ptr<Expression> interpolate(type::Type type, Interpolator interpolator, std::unique_ptr<Expression> input, std::map<double, std::unique_ptr<Expression>> stops) {
    ParsingContext ctx;
    auto result = createInterpolate(type, std::move(interpolator), std::move(input), std::move(stops), ctx);
    if (!result) {
        assert(false);
        return {};
    }
    return std::move(*result);
}

template <class T>
std::unique_ptr<Expression> categorical(type::Type type, const std::string& property, std::map<T, std::unique_ptr<Expression>> branches) {
    std::unordered_map<T, std::shared_ptr<Expression>> convertedBranches;
    for (auto& b : branches) {
        convertedBranches[b.first] = std::move(b.second);
    }
    return std::make_unique<Match<T>>(type, get(literal(property)), std::move(convertedBranches), error("replaced with default"));
}

template <>
std::unique_ptr<Expression> categorical<bool>(type::Type type, const std::string& property, std::map<bool, std::unique_ptr<Expression>> branches) {
    auto it = branches.find(true);
    std::unique_ptr<Expression> trueCase = it == branches.end() ?
        error("replaced with default") :
        std::move(it->second);

    it = branches.find(false);
    std::unique_ptr<Expression> falseCase = it == branches.end() ?
        error("replaced with default") :
        std::move(it->second);

    std::vector<typename Case::Branch> trueBranch;
    trueBranch.emplace_back(get(literal(property)), std::move(trueCase));

    return std::make_unique<Case>(type, std::move(trueBranch), std::move(falseCase));
}

static optional<std::unique_ptr<Expression>> convertIntervalFunction(type::Type type,
                                                                     const Convertible& value,
                                                                     Error& error,
                                                                     std::unique_ptr<Expression> input,
                                                                     bool convertTokens = false) {
    auto stops = convertStops(type, value, error, convertTokens);
    if (!stops) {
        return {};
    }
    return step(type, std::move(input), std::move(*stops));
}

static optional<std::unique_ptr<Expression>> convertExponentialFunction(type::Type type,
                                                                        const Convertible& value,
                                                                        Error& error,
                                                                        std::unique_ptr<Expression> input,
                                                                        bool convertTokens = false) {
    auto stops = convertStops(type, value, error, convertTokens);
    if (!stops) {
        return {};
    }
    auto base = convertBase(value, error);
    if (!base) {
        return {};
    }
    return interpolate(type, exponential(*base), std::move(input), std::move(*stops));
}

static optional<std::unique_ptr<Expression>> convertCategoricalFunction(type::Type type,
                                                                        const Convertible& value,
                                                                        Error& err,
                                                                        const std::string& property) {
    auto stopsValue = objectMember(value, "stops");
    if (!stopsValue) {
        err = { "function value must specify stops" };
        return {};
    }

    if (!isArray(*stopsValue)) {
        err = { "function stops must be an array" };
        return {};
    }

    if (arrayLength(*stopsValue) == 0) {
        err = { "function must have at least one stop" };
        return {};
    }

    const auto& first = arrayMember(*stopsValue, 0);

    if (!isArray(first)) {
        err = { "function stop must be an array" };
        return {};
    }

    if (arrayLength(first) != 2) {
        err = { "function stop must have two elements" };
        return {};
    }

    if (toBool(arrayMember(first, 0))) {
        auto branches = convertBranches<bool>(type, value, err);
        if (!branches) {
            return {};
        }
        return categorical(type, property, std::move(*branches));
    }

    if (toNumber(arrayMember(first, 0))) {
        auto branches = convertBranches<int64_t>(type, value, err);
        if (!branches) {
            return {};
        }
        return categorical(type, property, std::move(*branches));
    }

    if (toString(arrayMember(first, 0))) {
        auto branches = convertBranches<std::string>(type, value, err);
        if (!branches) {
            return {};
        }
        return categorical(type, property, std::move(*branches));
    }

    err = { "stop domain value must be a number, string, or boolean" };
    return {};
}

template <class T, class Fn>
optional<std::unique_ptr<Expression>> composite(type::Type type,
                                                const Convertible& value,
                                                Error& error,
                                                const Fn& makeInnerExpression) {
    auto base = convertBase(value, error);
    if (!base) {
        return {};
    }

    auto stopsValue = objectMember(value, "stops");

    // Checked by caller.
    assert(stopsValue);
    assert(isArray(*stopsValue));

    std::map<float, std::map<T, std::unique_ptr<Expression>>> map;

    for (std::size_t i = 0; i < arrayLength(*stopsValue); ++i) {
        const auto& stopValue = arrayMember(*stopsValue, i);

        if (!isArray(stopValue)) {
            error = { "function stop must be an array" };
            return {};
        }

        if (arrayLength(stopValue) != 2) {
            error = { "function stop must have two elements" };
            return {};
        }

        const auto& stopInput = arrayMember(stopValue, 0);

        if (!isObject(stopInput)) {
            error = { "stop input must be an object" };
            return {};
        }

        auto zoomValue = objectMember(stopInput, "zoom");
        if (!zoomValue) {
            error = { "stop input must specify zoom" };
            return {};
        }

        auto sourceValue = objectMember(stopInput, "value");
        if (!sourceValue) {
            error = { "stop input must specify value" };
            return {};
        }

        optional<float> z = convert<float>(*zoomValue, error);
        if (!z) {
            return {};
        }

        optional<T> d = convert<T>(*sourceValue, error);
        if (!d) {
            return {};
        }

        optional<std::unique_ptr<Expression>> r = convertLiteral(type, arrayMember(stopValue, 1), error);
        if (!r) {
            return {};
        }

        map[*z].emplace(*d, std::move(*r));
    }

    std::map<double, std::unique_ptr<Expression>> stops;

    for (auto& e : map) {
        stops.emplace(e.first, makeInnerExpression(type, *base, std::move(e.second)));
    }

    if (interpolatable(type)) {
        return interpolate(type, linear(), zoom(), std::move(stops));
    } else {
        return step(type, zoom(), std::move(stops));
    }
}

optional<std::unique_ptr<Expression>> convertFunctionToExpression(type::Type type,
                                                                  const Convertible& value,
                                                                  Error& err,
																  bool convertTokens) {
    if (!isObject(value)) {
        err = { "function must be an object" };
        return {};
    }

    FunctionType functionType = FunctionType::Invalid;

    auto typeValue = objectMember(value, "type");
    if (!typeValue) {
        functionType = interpolatable(type) ? FunctionType::Exponential : FunctionType::Interval;
    } else {
        optional<std::string> string = toString(*typeValue);
        if (string) {
            if (*string == "interval")
                functionType = FunctionType::Interval;
            if (*string == "exponential" && interpolatable(type))
                functionType = FunctionType::Exponential;
            if (*string == "categorical")
                functionType = FunctionType::Categorical;
            if (*string == "identity")
                functionType = FunctionType::Identity;
        }
    }

    if (!objectMember(value, "property")) {
        // Camera function.
        switch (functionType) {
        case FunctionType::Interval:
            return convertIntervalFunction(type, value, err, zoom(), convertTokens);
        case FunctionType::Exponential:
            return convertExponentialFunction(type, value, err, zoom(), convertTokens);
        default:
            err = { "unsupported function type" };
            return {};
        }
    }

    auto propertyValue = objectMember(value, "property");
    if (!propertyValue) {
        err = { "function must specify property" };
        return {};
    }

    auto property = toString(*propertyValue);
    if (!property) {
        err = { "function property must be a string" };
        return {};
    }

    if (functionType == FunctionType::Identity) {
        return type.match(
            [&] (const type::StringType&) -> optional<std::unique_ptr<Expression>> {
                return string(get(literal(*property)));
            },
            [&] (const type::NumberType&) -> optional<std::unique_ptr<Expression>> {
                return number(get(literal(*property)));
            },
            [&] (const type::BooleanType&) -> optional<std::unique_ptr<Expression>> {
                return boolean(get(literal(*property)));
            },
            [&] (const type::ColorType&) -> optional<std::unique_ptr<Expression>> {
                return toColor(get(literal(*property)));
            },
            [&] (const type::Array& array) -> optional<std::unique_ptr<Expression>> {
                return std::unique_ptr<Expression>(
                    std::make_unique<ArrayAssertion>(array, get(literal(*property))));
            },
            [&] (const auto&) -> optional<std::unique_ptr<Expression>>  {
                assert(false); // No properties use this type.
                return {};
            }
        );
    }

    auto stopsValue = objectMember(value, "stops");
    if (!stopsValue) {
        err = { "function value must specify stops" };
        return {};
    }

    if (!isArray(*stopsValue)) {
        err = { "function stops must be an array" };
        return {};
    }

    if (arrayLength(*stopsValue) == 0) {
        err = { "function must have at least one stop" };
        return {};
    }

    const auto& first = arrayMember(*stopsValue, 0);

    if (!isArray(first)) {
        err = { "function stop must be an array" };
        return {};
    }

    if (arrayLength(first) != 2) {
        err = { "function stop must have two elements" };
        return {};
    }

    const auto& stop = arrayMember(first, 0);

    if (!isObject(stop)) {
        // Source function.
        switch (functionType) {
        case FunctionType::Interval:
            return convertIntervalFunction(type, value, err, number(get(literal(*property))));
        case FunctionType::Exponential:
            return convertExponentialFunction(type, value, err, number(get(literal(*property))));
        case FunctionType::Categorical:
            return convertCategoricalFunction(type, value, err, *property);
        default:
            err = { "unsupported function type" };
            return {};
        }
    } else {
        // Composite function.
        auto sourceValue = objectMember(stop, "value");
        if (!sourceValue) {
            err = { "stop must specify value" };
            return {};
        }

        if (toBool(*sourceValue)) {
            switch (functionType) {
            case FunctionType::Categorical:
                return composite<bool>(type, value, err, [&] (type::Type type_, double, std::map<bool, std::unique_ptr<Expression>> stops) {
                    return categorical<bool>(type_, *property, std::move(stops));
                });
            default:
                err = { "unsupported function type" };
                return {};
            }
        }

        if (toNumber(*sourceValue)) {
            switch (functionType) {
            case FunctionType::Interval:
                return composite<double>(type, value, err, [&] (type::Type type_, double, std::map<double, std::unique_ptr<Expression>> stops) {
                    return step(type_, number(get(literal(*property))), std::move(stops));
                });
            case FunctionType::Exponential:
                return composite<double>(type, value, err, [&] (type::Type type_, double base, std::map<double, std::unique_ptr<Expression>> stops) {
                    return interpolate(type_, exponential(base), number(get(literal(*property))), std::move(stops));
                });
            case FunctionType::Categorical:
                return composite<int64_t>(type, value, err, [&] (type::Type type_, double, std::map<int64_t, std::unique_ptr<Expression>> stops) {
                    return categorical<int64_t>(type_, *property, std::move(stops));
                });
            default:
                err = { "unsupported function type" };
                return {};
            }
        }

        if (toString(*sourceValue)) {
            switch (functionType) {
            case FunctionType::Categorical:
                return composite<std::string>(type, value, err, [&] (type::Type type_, double, std::map<std::string, std::unique_ptr<Expression>> stops) {
                    return categorical<std::string>(type_, *property, std::move(stops));
                });
            default:
                err = { "unsupported function type" };
                return {};
            }
        }

        err = { "stop domain value must be a number, string, or boolean" };
        return {};
    }
}

} // namespace conversion
} // namespace style
} // namespace mbgl
