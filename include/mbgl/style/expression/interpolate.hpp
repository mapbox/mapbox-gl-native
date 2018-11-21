#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/parsing_context.hpp>
#include <mbgl/style/expression/get_covering_stops.hpp>
#include <mbgl/style/expression/interpolator.hpp>
#include <mbgl/style/conversion.hpp>

#include <memory>
#include <map>
#include <cmath>

namespace mbgl {
namespace style {
namespace expression {

ParseResult parseInterpolate(const mbgl::style::conversion::Convertible& value, ParsingContext& ctx);

class Interpolate : public Expression {
public:
    Interpolate(const type::Type& type_,
                Interpolator interpolator_,
                std::unique_ptr<Expression> input_,
                std::map<double, std::unique_ptr<Expression>> stops_);

    const std::unique_ptr<Expression>& getInput() const { return input; }
    const Interpolator& getInterpolator() const { return interpolator; }

    void eachChild(const std::function<void(const Expression&)>& visit) const override {
        visit(*input);
        for (const auto& stop : stops) {
            visit(*stop.second);
        }
    }

    void eachStop(const std::function<void(double, const Expression&)>& visit) const {
        for (const auto& stop : stops) {
            visit(stop.first, *stop.second);
        }
    }
    
    // Return the smallest range of stops that covers the interval [lower, upper]
    Range<float> getCoveringStops(const double lower, const double upper) const {
        return ::mbgl::style::expression::getCoveringStops(stops, lower, upper);
    }
    
    double interpolationFactor(const Range<double>& inputLevels, const double inputValue) const {
        return interpolator.match(
            [&](const auto& interp) { return interp.interpolationFactor(inputLevels, inputValue); }
        );
    }

    bool operator==(const Expression& e) const override {
        if (e.getKind() == Kind::Interpolate) {
            auto rhs = static_cast<const Interpolate*>(&e);
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

    std::vector<optional<Value>> possibleOutputs() const override;
    mbgl::Value serialize() const override;
    std::string getOperator() const override { return "interpolate"; }

protected:
    const Interpolator interpolator;
    const std::unique_ptr<Expression> input;
    const std::map<double, std::unique_ptr<Expression>> stops;
};

ParseResult createInterpolate(type::Type type,
                              Interpolator interpolator,
                              std::unique_ptr<Expression> input,
                              std::map<double, std::unique_ptr<Expression>> stops,
                              ParsingContext& ctx);

} // namespace expression
} // namespace style
} // namespace mbgl
