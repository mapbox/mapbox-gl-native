#pragma once

#include <mbgl/style/expression/array_assertion.hpp>
#include <mbgl/style/expression/assertion.hpp>
#include <mbgl/style/expression/case.hpp>
#include <mbgl/style/expression/coalesce.hpp>
#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/coercion.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/literal.hpp>
#include <mbgl/style/expression/match.hpp>
#include <mbgl/style/expression/step.hpp>

#include <mbgl/style/function/exponential_stops.hpp>
#include <mbgl/style/function/interval_stops.hpp>
#include <mbgl/style/function/categorical_stops.hpp>
#include <mbgl/style/function/composite_exponential_stops.hpp>
#include <mbgl/style/function/composite_interval_stops.hpp>
#include <mbgl/style/function/composite_categorical_stops.hpp>
#include <mbgl/style/function/identity_stops.hpp>

#include <mbgl/util/enum.hpp>
#include <mbgl/style/types.hpp>

#include <string>


namespace mbgl {
namespace style {
namespace expression {

namespace detail {

class ErrorExpression : public Expression {
public:
    ErrorExpression(std::string message_) : Expression(type::Error), message(std::move(message_)) {}
    void eachChild(const std::function<void(const Expression&)>&) const override {}
    
    bool operator==(const Expression& e) const override {
        return dynamic_cast<const ErrorExpression*>(&e);
    }

    EvaluationResult evaluate(const EvaluationContext&) const override {
        return EvaluationError{message};
    }

    std::vector<optional<Value>> possibleOutputs() const override {
        return {};
    }

    std::string getOperator() const override { return "error"; }
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
    
    static std::unique_ptr<Expression> makeGet(type::Type type, const std::string& property) {
        ParsingContext ctx;
        std::vector<std::unique_ptr<Expression>> getArgs;
        getArgs.push_back(makeLiteral(property));
        ParseResult get = createCompoundExpression("get", std::move(getArgs), ctx);
        assert(get);
        assert(ctx.getErrors().size() == 0);

        std::vector<std::unique_ptr<Expression>> assertionArgs;
        assertionArgs.push_back(std::move(*get));
        
        return std::make_unique<Assertion>(type, std::move(assertionArgs));
    }
    
    static std::unique_ptr<Expression> makeZoom() {
        ParsingContext ctx;
        ParseResult zoom = createCompoundExpression("zoom", std::vector<std::unique_ptr<Expression>>(), ctx);
        assert(zoom);
        assert(ctx.getErrors().size() == 0);
        return std::move(*zoom);
    }
    
    static std::unique_ptr<Expression> makeError(std::string message) {
        return std::make_unique<detail::ErrorExpression>(message);
    }
    
    template <typename OutputType>
    static ParseResult makeInterpolate(type::Type type,
                                 std::unique_ptr<Expression> input,
                                 std::map<double, std::unique_ptr<Expression>> convertedStops,
                                 typename Interpolate<OutputType>::Interpolator interpolator)
    {
        ParseResult curve = ParseResult(std::make_unique<Interpolate<OutputType>>(
            std::move(type),
            std::move(interpolator),
            std::move(input),
            std::move(convertedStops)
        ));
        assert(curve);
        return std::move(*curve);
    }
    
    template <typename Key>
    static ParseResult makeMatch(type::Type type,
                                 std::unique_ptr<Expression> input,
                                 std::map<CategoricalValue, std::unique_ptr<Expression>> stops) {
        // match expression
        typename Match<Key>::Branches branches;
        for(auto it = stops.begin(); it != stops.end(); it++) {
            assert(it->first.template is<Key>());
            Key key = it->first.template get<Key>();
            branches.emplace(
                std::move(key),
                std::move(it->second)
            );
        }
        
        return ParseResult(std::make_unique<Match<Key>>(std::move(type),
                                            std::move(input),
                                            std::move(branches),
                                            makeError("No matching label")));
    }
    
    static ParseResult makeCase(type::Type type,
                                std::unique_ptr<Expression> input,
                                std::map<CategoricalValue, std::unique_ptr<Expression>> stops) {
        // case expression
        std::vector<typename Case::Branch> branches;
        
        auto it = stops.find(true);
        std::unique_ptr<Expression> true_case = it == stops.end() ?
            makeError("No matching label") :
            std::move(it->second);

        it = stops.find(false);
        std::unique_ptr<Expression> false_case = it == stops.end() ?
            makeError("No matching label") :
            std::move(it->second);

        branches.push_back(std::make_pair(std::move(input), std::move(true_case)));
        return ParseResult(std::make_unique<Case>(std::move(type), std::move(branches), std::move(false_case)));
    }
    
    template <typename T>
    static ParseResult fromCategoricalStops(std::map<CategoricalValue, T> stops, const std::string& property) {
        assert(stops.size() > 0);

        std::map<CategoricalValue, std::unique_ptr<Expression>> convertedStops;
        for(const std::pair<CategoricalValue, T>& stop : stops) {
            convertedStops.emplace(
                stop.first,
                makeLiteral(stop.second)
            );
        }
        
        type::Type type = valueTypeToExpressionType<T>();

        const CategoricalValue& firstKey = stops.begin()->first;
        return firstKey.match(
            [&](bool) {
                return makeCase(type, makeGet(type::Boolean, property), std::move(convertedStops));
            },
            [&](const std::string&) {
                return makeMatch<std::string>(type, makeGet(type::String, property), std::move(convertedStops));
            },
            [&](int64_t) {
                return makeMatch<int64_t>(type, makeGet(type::Number, property), std::move(convertedStops));
            }
        );
    }
    
    template <typename T>
    static std::map<double, std::unique_ptr<Expression>> convertStops(const std::map<float, T>& stops) {
        std::map<double, std::unique_ptr<Expression>> convertedStops;
        for(const auto& stop : stops) {
            convertedStops.emplace(
                stop.first,
                makeLiteral(stop.second)
            );
        }
        return convertedStops;
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const ExponentialStops<T>& stops)
    {
        ParseResult e = makeInterpolate<typename ValueConverter<T>::ExpressionType>(
                                         valueTypeToExpressionType<T>(),
                                         makeZoom(),
                                         convertStops(stops.stops),
                                         ExponentialInterpolator(stops.base));
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const IntervalStops<T>& stops)
    {
        ParseResult e(std::make_unique<Step>(valueTypeToExpressionType<T>(),
                                             makeZoom(),
                                             convertStops(stops.stops)));
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const ExponentialStops<T>& stops)
    {
        ParseResult e = makeInterpolate<typename ValueConverter<T>::ExpressionType>(valueTypeToExpressionType<T>(),
                                                                     makeGet(type::Number, property),
                                                                     convertStops(stops.stops),
                                                                     ExponentialInterpolator(stops.base));
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const IntervalStops<T>& stops)
    {
        std::unique_ptr<Expression> get = makeGet(type::Number, property);
        ParseResult e(std::make_unique<Step>(valueTypeToExpressionType<T>(),
                                             std::move(get),
                                             convertStops(stops.stops)));
        assert(e);
        return std::move(*e);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CategoricalStops<T>& stops)
    {
        ParseResult expr = fromCategoricalStops(stops.stops, property);
        assert(expr);
        return std::move(*expr);
    }

    // interpolatable zoom curve
    template <typename T>
    static typename std::enable_if_t<util::Interpolatable<T>::value,
    ParseResult> makeZoomCurve(std::map<double, std::unique_ptr<Expression>> stops) {
        return makeInterpolate<typename ValueConverter<T>::ExpressionType>(valueTypeToExpressionType<T>(),
                                                                           makeZoom(),
                                                                           std::move(stops),
                                                                           ExponentialInterpolator(1.0));
    }
    
    // non-interpolatable zoom curve
    template <typename T>
    static typename std::enable_if_t<!util::Interpolatable<T>::value,
    ParseResult> makeZoomCurve(std::map<double, std::unique_ptr<Expression>> stops) {
        return ParseResult(std::make_unique<Step>(valueTypeToExpressionType<T>(), makeZoom(), std::move(stops)));
    }

    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CompositeExponentialStops<T>& stops)
    {
        std::map<double, std::unique_ptr<Expression>> outerStops;
        for (const std::pair<float, std::map<float, T>>& stop : stops.stops) {
            std::unique_ptr<Expression> get = makeGet(type::Number, property);
            ParseResult innerInterpolate = makeInterpolate<typename ValueConverter<T>::ExpressionType>(valueTypeToExpressionType<T>(),
                                                                                  std::move(get),
                                                                                  convertStops(stop.second),
                                                                                  ExponentialInterpolator(stops.base));
            assert(innerInterpolate);
            outerStops.emplace(stop.first, std::move(*innerInterpolate));
        }
        
        ParseResult zoomCurve = makeZoomCurve<T>(std::move(outerStops));
        assert(zoomCurve);
        return std::move(*zoomCurve);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CompositeIntervalStops<T>& stops)
    {
        std::map<double, std::unique_ptr<Expression>> outerStops;
        for (const std::pair<float, std::map<float, T>>& stop : stops.stops) {
            std::unique_ptr<Expression> get = makeGet(type::Number, property);
            ParseResult innerInterpolate(std::make_unique<Step>(valueTypeToExpressionType<T>(),
                                                                std::move(get),
                                                                convertStops(stop.second)));
            assert(innerInterpolate);
            outerStops.emplace(stop.first, std::move(*innerInterpolate));
        }

        ParseResult zoomCurve = makeZoomCurve<T>(std::move(outerStops));
        assert(zoomCurve);
        return std::move(*zoomCurve);
    }
    
    template <typename T>
    static std::unique_ptr<Expression> toExpression(const std::string& property,
                                                  const CompositeCategoricalStops<T>& stops)
    {
        std::map<double, std::unique_ptr<Expression>> outerStops;
        for (const std::pair<float, std::map<CategoricalValue, T>>& stop : stops.stops) {
            ParseResult innerInterpolate = fromCategoricalStops(stop.second, property);
            assert(innerInterpolate);
            outerStops.emplace(stop.first, std::move(*innerInterpolate));
        }
        
        ParseResult zoomCurve = makeZoomCurve<T>(std::move(outerStops));
        assert(zoomCurve);
        return std::move(*zoomCurve);
    }

    
    static std::unique_ptr<Expression> fromIdentityFunction(type::Type type, const std::string& property)
    {
        std::unique_ptr<Expression> input = type.match(
            [&] (const type::StringType&) {
                return makeGet(type::String, property);
            },
            [&] (const type::NumberType&) {
                return makeGet(type::Number, property);
            },
            [&] (const type::BooleanType&) {
                return makeGet(type::Boolean, property);
            },
            [&] (const type::ColorType&) {
                std::vector<std::unique_ptr<Expression>> args;
                args.push_back(makeGet(type::String, property));
                return std::make_unique<Coercion>(type::Color, std::move(args));
            },
            [&] (const type::Array& arr) {
                std::vector<std::unique_ptr<Expression>> getArgs;
                getArgs.push_back(makeLiteral(property));
                ParsingContext ctx;
                ParseResult get = createCompoundExpression("get", std::move(getArgs), ctx);
                assert(get);
                assert(ctx.getErrors().size() == 0);
                return std::make_unique<ArrayAssertion>(arr, std::move(*get));
            },
            [&] (const auto&) -> std::unique_ptr<Expression> {
                return makeLiteral(Null);
            }
        );
        
        return input;
    }
};

} // namespace expression
} // namespace style
} // namespace mbgl
