#pragma once

#include <mbgl/util/enum.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/expression/array_assertion.hpp>
#include <mbgl/style/expression/case.hpp>
#include <mbgl/style/expression/coalesce.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/curve.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/match.hpp>

#include <mbgl/style/function/exponential_stops.hpp>
#include <mbgl/style/function/interval_stops.hpp>
#include <mbgl/style/function/categorical_stops.hpp>
#include <mbgl/style/function/composite_exponential_stops.hpp>
#include <mbgl/style/function/composite_interval_stops.hpp>
#include <mbgl/style/function/composite_categorical_stops.hpp>
#include <mbgl/style/function/identity_stops.hpp>

#include <string>


namespace mbgl {
namespace style {
namespace expression {

namespace detail {

class ErrorExpression : public Expression {
public:
    ErrorExpression(std::string message_) : Expression(type::Error), message(std::move(message_)) {}
    void accept(std::function<void(const Expression*)> visit) const override {
        visit(this);
    }

    EvaluationResult evaluate(const EvaluationParameters&) const override {
        return EvaluationError{message};
    }

private:
    std::string message;
};

} // namespace detail


// Create expressions representing 'classic' (i.e. stop-based) style functions

struct Convert {
    template <typename T>
    static std::unique_ptr<Literal> makeLiteral(const T& value) {
        return std::make_unique<Literal>(Value(toExpressionValue(value)));
    }
    
    static std::unique_ptr<Expression> makeGet(const std::string& type, const std::string& property, ParsingContext ctx) {
        std::vector<std::unique_ptr<Expression>> getArgs;
        getArgs.push_back(makeLiteral(property));
        ParseResult get = createCompoundExpression("get", std::move(getArgs), ctx);

        std::vector<std::unique_ptr<Expression>> assertionArgs;
        assertionArgs.push_back(std::move(*get));
        
        return std::move(*(createCompoundExpression(type, std::move(assertionArgs), ctx)));
    }
    
    static std::unique_ptr<Expression> makeZoom(ParsingContext ctx) {
        return std::move(*(createCompoundExpression("zoom", std::vector<std::unique_ptr<Expression>>(), ctx)));
    }
    
    static std::unique_ptr<Expression> makeError(std::string message) {
        return std::make_unique<detail::ErrorExpression>(message);
    }


    template <typename T>
    static ParseResult makeCoalesceToDefault(std::unique_ptr<Expression> main, optional<T> defaultValue) {
        if (!defaultValue) {
            return ParseResult(std::move(main));
        }
        
        Coalesce::Args args;
        args.push_back(std::move(main));
        args.push_back(makeLiteral(*defaultValue));
        return ParseResult(std::make_unique<Coalesce>(valueTypeToExpressionType<T>(), std::move(args)));
    }
    
    template <typename T>
    static std::map<double, std::unique_ptr<Expression>> convertStops(const std::map<float, T>& stops) {
        std::map<double, std::unique_ptr<Expression>> convertedStops;
        for(const std::pair<float, T>& stop : stops) {
            convertedStops.emplace(
                stop.first,
                makeLiteral(stop.second)
            );
        }
        return convertedStops;
    }
    
    template <typename T>
    static ParseResult makeCurve(std::unique_ptr<Expression> input,
                                 std::map<double, std::unique_ptr<Expression>> convertedStops,
                                 typename Curve<typename ValueConverter<T>::ExpressionType>::Interpolator interpolator,
                                 optional<T> defaultValue)
    {
        ParseResult curve = ParseResult(std::make_unique<Curve<typename ValueConverter<T>::ExpressionType>>(
            valueTypeToExpressionType<T>(),
            std::move(interpolator),
            std::move(input),
            std::move(convertedStops)
        ));
        assert(curve);
        return makeCoalesceToDefault(std::move(*curve), defaultValue);
    }
    
    template <typename Key, typename T>
    static ParseResult makeMatch(std::unique_ptr<Expression> input,
                                 const std::map<CategoricalValue, T>& stops) {
        // match expression
        typename Match<Key>::Branches branches;
        for(const std::pair<CategoricalValue, T>& stop : stops) {
            assert(stop.first.template is<Key>());
            Key key = stop.first.template get<Key>();
            branches.emplace(
                std::move(key),
                makeLiteral(stop.second)
            );
        }
        
        return ParseResult(std::make_unique<Match<Key>>(valueTypeToExpressionType<T>(),
                                            std::move(input),
                                            std::move(branches),
                                            makeError("No matching label")));
    }
    
    template <typename T>
    static ParseResult makeCase(std::unique_ptr<Expression> input,
                                const std::map<CategoricalValue, T>& stops) {
        // case expression
        std::vector<typename Case::Branch> branches;
        
        auto it = stops.find(true);
        std::unique_ptr<Expression> true_case = it == stops.end() ?
            makeError("No matching label") :
            makeLiteral(it->second);

        it = stops.find(false);
        std::unique_ptr<Expression> false_case = it == stops.end() ?
            makeError("No matching label") :
            makeLiteral(it->second);

        branches.push_back(std::make_pair(std::move(input), std::move(true_case)));
        return ParseResult(std::make_unique<Case>(valueTypeToExpressionType<T>(), std::move(branches), std::move(false_case)));
    }
    
    template <typename T>
    static ParseResult convertCategoricalStops(std::map<CategoricalValue, T> stops, const std::string& property) {
        assert(stops.size() > 0);

        std::vector<ParsingError> errors;
        ParsingContext ctx(errors);

        const CategoricalValue& firstKey = stops.begin()->first;
        return firstKey.match(
            [&](bool) {
                return makeCase(makeGet("boolean", property, ctx), stops);
            },
            [&](const std::string&) {
                return makeMatch<std::string>(makeGet("string", property, ctx), stops);
            },
            [&](int64_t) {
                return makeMatch<int64_t>(makeGet("number", property, ctx), stops);
            }
        );
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const ExponentialStops<T>& stops)
    {
        std::vector<ParsingError> errors;
        ParseResult e = makeCurve(makeZoom(ParsingContext(errors)),
                                           convertStops(stops.stops),
                                           ExponentialInterpolator(stops.base),
                                           optional<T>());
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const IntervalStops<T>& stops)
    {
        std::vector<ParsingError> errors;
        ParseResult e = makeCurve(makeZoom(ParsingContext(errors)), convertStops(stops.stops), StepInterpolator(), optional<T>());
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const ExponentialStops<T>& stops,
                                                  optional<T> defaultValue)
    {
        std::vector<ParsingError> errors;
        ParseResult e = makeCurve(makeGet("number", property, ParsingContext(errors)),
                                          convertStops(stops.stops),
                                          ExponentialInterpolator(stops.base),
                                          defaultValue);
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const IntervalStops<T>& stops,
                                                  optional<T> defaultValue)
    {
        std::vector<ParsingError> errors;
        std::unique_ptr<Expression> get = makeGet("number", property, ParsingContext(errors));
        ParseResult e = makeCurve(std::move(get), convertStops(stops.stops), StepInterpolator(), defaultValue);
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CategoricalStops<T>& stops,
                                                  optional<T> defaultValue)
    {
        ParseResult expr = convertCategoricalStops(stops.stops, property);
        assert(expr);
        ParseResult e = makeCoalesceToDefault(std::move(*expr), defaultValue);
        assert(e);
        return std::move(*e);
    }

    template <typename T>
    static typename Curve<std::enable_if_t<util::Interpolatable<T>::value, T>>::Interpolator zoomInterpolator() {
        return ExponentialInterpolator(1.0);
    }
    
    template <typename T>
    static typename Curve<std::enable_if_t<!util::Interpolatable<T>::value, T>>::Interpolator zoomInterpolator() {
        return StepInterpolator();
    }


    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CompositeExponentialStops<T>& stops,
                                                  optional<T> defaultValue)
    {
        std::vector<ParsingError> errors;
        std::map<double, std::unique_ptr<Expression>> outerStops;
        for (const std::pair<float, std::map<float, T>>& stop : stops.stops) {
            std::unique_ptr<Expression> get = makeGet("number", property, ParsingContext(errors));
            ParseResult innerCurve = makeCurve(std::move(get),
                                               convertStops(stop.second),
                                               ExponentialInterpolator(stops.base),
                                               defaultValue);
            assert(innerCurve);
            outerStops.emplace(stop.first, std::move(*innerCurve));
        }
        ParseResult outerCurve = makeCurve(makeZoom(ParsingContext(errors)),
                                           std::move(outerStops),
                                           zoomInterpolator<T>(),
                                           defaultValue);
        assert(outerCurve);
        ParseResult e = makeCoalesceToDefault(std::move(*outerCurve), defaultValue);
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CompositeIntervalStops<T>& stops,
                                                  optional<T> defaultValue)
    {
        std::vector<ParsingError> errors;
        std::map<double, std::unique_ptr<Expression>> outerStops;
        for (const std::pair<float, std::map<float, T>>& stop : stops.stops) {
            std::unique_ptr<Expression> get = makeGet("number", property, ParsingContext(errors));
            ParseResult innerCurve = makeCurve(std::move(get), convertStops(stop.second), StepInterpolator(), defaultValue);
            assert(innerCurve);
            outerStops.emplace(stop.first, std::move(*innerCurve));
        }
        ParseResult outerCurve = makeCurve(makeZoom(ParsingContext(errors)),
                                           std::move(outerStops),
                                           zoomInterpolator<T>(),
                                           defaultValue);
        assert(outerCurve);
        ParseResult e = makeCoalesceToDefault(std::move(*outerCurve), defaultValue);
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CompositeCategoricalStops<T>& stops,
                                                  optional<T> defaultValue)
    {
        std::vector<ParsingError> errors;
        std::map<double, std::unique_ptr<Expression>> outerStops;
        for (const std::pair<float, std::map<CategoricalValue, T>>& stop : stops.stops) {
            ParseResult innerCurve = convertCategoricalStops(stop.second, property);
            assert(innerCurve);
            outerStops.emplace(stop.first, std::move(*innerCurve));
        }
        ParseResult outerCurve = makeCurve(makeZoom(ParsingContext(errors)),
                                           std::move(outerStops),
                                           zoomInterpolator<T>(),
                                           defaultValue);
        assert(outerCurve);
        ParseResult e = makeCoalesceToDefault(std::move(*outerCurve), defaultValue);
        assert(e);
        return std::move(*e);
    }

    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const IdentityStops<T>&,
                                                  optional<T> defaultValue)
    {
        std::vector<ParsingError> errors;

        std::unique_ptr<Expression> input = valueTypeToExpressionType<T>().match(
            [&] (const type::StringType&) {
                return makeGet("string", property, ParsingContext(errors));
            },
            [&] (const type::NumberType&) {
                return makeGet("number", property, ParsingContext(errors));
            },
            [&] (const type::BooleanType&) {
                return makeGet("boolean", property, ParsingContext(errors));
            },
            [&] (const type::ColorType&) {
                std::vector<std::unique_ptr<Expression>> args;
                args.push_back(makeGet("string", property, ParsingContext(errors)));
                ParseResult color = createCompoundExpression("to-color", std::move(args), ParsingContext(errors));
                assert(color);
                return std::move(*color);
            },
            [&] (const type::Array& arr) {
                std::vector<std::unique_ptr<Expression>> getArgs;
                getArgs.push_back(makeLiteral(property));
                ParseResult get = createCompoundExpression("get", std::move(getArgs), ParsingContext(errors));
                return std::make_unique<ArrayAssertion>(arr, std::move(*get));
            },
            [&] (const auto&) -> std::unique_ptr<Expression> {
                return makeLiteral(Null);
            }
        );
        
        ParseResult e = makeCoalesceToDefault(std::move(input), defaultValue);
        assert(e);
        return std::move(*e);
    }
};

} // namespace expression
} // namespace style
} // namespace mbgl
