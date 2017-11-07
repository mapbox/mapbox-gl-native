#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/get_covering_stops.hpp>
#include <mbgl/style/conversion.hpp>

#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/range.hpp>
#include <mbgl/util/unitbezier.hpp>

#include <memory>
#include <map>


namespace mbgl {
namespace style {
namespace expression {


class ExponentialInterpolator {
public:
    ExponentialInterpolator(double base_) : base(base_) {}

    double base;
    
    double interpolationFactor(const Range<double>& inputLevels, const double input) const {
        return util::interpolationFactor(base,
                                         Range<float> {
                                            static_cast<float>(inputLevels.min),
                                            static_cast<float>(inputLevels.max)
                                         },
                                         input);
    }
    
    bool operator==(const ExponentialInterpolator& rhs) const {
        return base == rhs.base;
    }
};

class CubicBezierInterpolator {
public:
    CubicBezierInterpolator(double x1_, double y1_, double x2_, double y2_) : ub(x1_, y1_, x2_, y2_) {}
    
    double interpolationFactor(const Range<double>& inputLevels, const double input) const {
        return ub.solve(input / (inputLevels.max - inputLevels.min), 1e-6);
    }
    
    bool operator==(const CubicBezierInterpolator& rhs) const {
        return ub == rhs.ub;
    }
    
    util::UnitBezier ub;
};


ParseResult parseInterpolate(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);


template <typename T>
class Interpolate : public Expression {
public:
    using Interpolator = variant<ExponentialInterpolator, CubicBezierInterpolator>;
    
    Interpolate(const type::Type& type_,
          Interpolator interpolator_,
          std::unique_ptr<Expression> input_,
          std::map<double, std::unique_ptr<Expression>> stops_
    ) : Expression(type_),
        interpolator(std::move(interpolator_)),
        input(std::move(input_)),
        stops(std::move(stops_))
    {
        static_assert(util::Interpolatable<T>::value, "Interpolate expression requires an interpolatable value type.");
    }
    
    EvaluationResult evaluate(const EvaluationContext& params) const override {
        const EvaluationResult evaluatedInput = input->evaluate(params);
        if (!evaluatedInput) { return evaluatedInput.error(); }
        float x = *fromExpressionValue<float>(*evaluatedInput);
        
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

            return interpolate({*lower, *upper}, t);
        }
        
    }
    
    void eachChild(const std::function<void(const Expression&)>& visit) const override {
        visit(*input);
        for (auto it = stops.begin(); it != stops.end(); it++) {
            visit(*(it->second));
        }
    }
    
    bool operator==(const Expression& e) const override {
        if (auto rhs = dynamic_cast<const Interpolate*>(&e)) {
            if (interpolator != rhs->interpolator ||
                *input != *(rhs->input) ||
                stops.size() != rhs->stops.size())
            {
                return false;
            }
            
            return Expression::childrenEqual(stops, rhs->stops);
        }
        return false;
    }
    
    const std::unique_ptr<Expression>& getInput() const { return input; }

    // Return the smallest range of stops that covers the interval [lower, upper]
    Range<float> getCoveringStops(const double lower, const double upper) const {
        return ::mbgl::style::expression::getCoveringStops(stops, lower, upper);
    }
    
    double interpolationFactor(const Range<double>& inputLevels, const double inputValue) const {
        return interpolator.match(
            [&](const auto& interp) { return interp.interpolationFactor(inputLevels, inputValue); }
        );
    }
    
private:
    static EvaluationResult interpolate(const Range<Value>& outputs, const double t) {
        optional<T> lower = fromExpressionValue<T>(outputs.min);
        if (!lower) {
            // TODO - refactor fromExpressionValue to return EvaluationResult<T> so as to
            // consolidate DRY up producing this error message.
            return EvaluationError {
                "Expected value to be of type " + toString(valueTypeToExpressionType<T>()) +
                ", but found " + toString(typeOf(outputs.min)) + " instead."
            };
        }
        const optional<T> upper = fromExpressionValue<T>(outputs.max);
        if (!upper) {
            return EvaluationError {
                "Expected value to be of type " + toString(valueTypeToExpressionType<T>()) +
                ", but found " + toString(typeOf(outputs.min)) + " instead."
            };
        }
        T result = util::interpolate(*lower, *upper, t);
        return toExpressionValue(result);
    }
    
    const Interpolator interpolator;
    const std::unique_ptr<Expression> input;
    const std::map<double, std::unique_ptr<Expression>> stops;
};

} // namespace expression
} // namespace style
} // namespace mbgl
