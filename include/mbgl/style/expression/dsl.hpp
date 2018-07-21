#pragma once

#include <mbgl/style/expression/value.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/interpolator.hpp>
#include <mbgl/util/ignore.hpp>

#include <memory>
#include <string>
#include <initializer_list>

namespace mbgl {
namespace style {
namespace expression {
namespace dsl {

// This convenience API does little to no expression validation or type-checking, and is intended for
// use only by test and other non-production code.

template <class... Args>
std::vector<std::unique_ptr<Expression>> vec(Args... args) {
    std::vector<std::unique_ptr<Expression>> result;
    util::ignore({ (result.push_back(std::move(args)), 0)... });
    return result;
}

std::unique_ptr<Expression> error(std::string);

std::unique_ptr<Expression> literal(const char* value);
std::unique_ptr<Expression> literal(Value value);
std::unique_ptr<Expression> literal(std::initializer_list<double> value);
std::unique_ptr<Expression> literal(std::initializer_list<const char *> value);

std::unique_ptr<Expression> number(std::unique_ptr<Expression>);
std::unique_ptr<Expression> string(std::unique_ptr<Expression>);
std::unique_ptr<Expression> boolean(std::unique_ptr<Expression>);

std::unique_ptr<Expression> toColor(std::unique_ptr<Expression>);
std::unique_ptr<Expression> toString(std::unique_ptr<Expression>);

std::unique_ptr<Expression> get(const char* value);
std::unique_ptr<Expression> get(std::unique_ptr<Expression>);

std::unique_ptr<Expression> id();
std::unique_ptr<Expression> zoom();

std::unique_ptr<Expression> eq(std::unique_ptr<Expression>,
                               std::unique_ptr<Expression>);
std::unique_ptr<Expression> ne(std::unique_ptr<Expression>,
                               std::unique_ptr<Expression>);
std::unique_ptr<Expression> gt(std::unique_ptr<Expression>,
                               std::unique_ptr<Expression>);
std::unique_ptr<Expression> lt(std::unique_ptr<Expression>,
                               std::unique_ptr<Expression>);

std::unique_ptr<Expression> step(std::unique_ptr<Expression> input,
                                 std::unique_ptr<Expression> output0,
                                 double input1, std::unique_ptr<Expression> output1);

Interpolator linear();
Interpolator exponential(double base);
Interpolator cubicBezier(double x1, double y1, double x2, double y2);

std::unique_ptr<Expression> interpolate(Interpolator interpolator,
                                        std::unique_ptr<Expression> input,
                                        double input1, std::unique_ptr<Expression> output1);

std::unique_ptr<Expression> interpolate(Interpolator interpolator,
                                        std::unique_ptr<Expression> input,
                                        double input1, std::unique_ptr<Expression> output1,
                                        double input2, std::unique_ptr<Expression> output2);

std::unique_ptr<Expression> interpolate(Interpolator interpolator,
                                        std::unique_ptr<Expression> input,
                                        double input1, std::unique_ptr<Expression> output1,
                                        double input2, std::unique_ptr<Expression> output2,
                                        double input3, std::unique_ptr<Expression> output3);

std::unique_ptr<Expression> concat(std::vector<std::unique_ptr<Expression>> inputs);

} // namespace dsl
} // namespace expression
} // namespace style
} // namespace mbgl
