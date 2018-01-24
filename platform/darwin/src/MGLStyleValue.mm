#import "MGLStyleValue_Private.h"

#include <mbgl/style/expression/expression.hpp>

const MGLStyleFunctionOption MGLStyleFunctionOptionInterpolationBase = @"MGLStyleFunctionOptionInterpolationBase";
const MGLStyleFunctionOption MGLStyleFunctionOptionDefaultValue = @"MGLStyleFunctionOptionDefaultValue";

id MGLJSONObjectFromMBGLValue(const mbgl::style::expression::Value &value) {
    return value.match([](const mbgl::NullValue) -> id {
        return [NSNull null];
    }, [](const bool value) {
        return @(value);
    }, [](const float value) {
        return @(value);
    }, [](const int64_t value) {
        return @(value);
    }, [](const double value) {
        return @(value);
    }, [](const std::string &value) {
        return @(value.c_str());
    }, [](const mbgl::Color &value) {
        return [MGLColor mgl_colorWithColor:value];
    }, [](const mbgl::style::Position &value) {
        std::array<float, 3> spherical = value.getSpherical();
        MGLSphericalPosition position = MGLSphericalPositionMake(spherical[0], spherical[1], spherical[2]);
        return [NSValue valueWithMGLSphericalPosition:position];
    }, [&](const std::vector<mbgl::style::expression::Value> &vector) {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:vector.size()];
        for (auto value : vector) {
            [array addObject:MGLJSONObjectFromMBGLValue(value)];
        }
        return @[@"literal", array];
    }, [&](const std::unordered_map<std::string, mbgl::style::expression::Value> &map) {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionaryWithCapacity:map.size()];
        for (auto &item : map) {
            dictionary[@(item.first.c_str())] = MGLJSONObjectFromMBGLValue(item.second);
        }
        return @[@"literal", dictionary];
    }, [](const auto &) -> id {
        return nil;
    });
}

id MGLJSONObjectFromMBGLExpression(const mbgl::style::expression::Expression &mbglExpression) {
    using namespace mbgl::style::expression;
    if (auto literalExpression = dynamic_cast<const Literal *>(&mbglExpression)) {
        auto result = literalExpression->evaluate({ nullptr });
        return result ? MGLJSONObjectFromMBGLValue(*result) : nil;
    }
    if (auto assertExpression = dynamic_cast<const ArrayAssertion *>(&mbglExpression)) {
        NSMutableArray *inputs = [NSMutableArray array];
        assertExpression->eachChild([&](const Expression &child) {
            [inputs addObject:MGLJSONObjectFromMBGLExpression(child)];
        });
        return @[@"literal", inputs.lastObject];
    }
    if (auto assertExpression = dynamic_cast<const Assertion *>(&mbglExpression)) {
        NSMutableArray *inputs = [NSMutableArray array];
        assertExpression->eachChild([&](const Expression &child) {
            [inputs addObject:MGLJSONObjectFromMBGLExpression(child)];
        });
        return inputs.firstObject;
    }
    if (auto compoundExpression = dynamic_cast<const CompoundExpressionBase *>(&mbglExpression)) {
        const std::string name = compoundExpression->getName();
        mbgl::optional<std::size_t> parameterCount = compoundExpression->getParameterCount();
        NSMutableArray *expressionObject = parameterCount ? [NSMutableArray arrayWithCapacity:*parameterCount + 1] : [NSMutableArray array];
        [expressionObject addObject:@(name.c_str())];
        compoundExpression->eachChild([&](const Expression &child) {
            [expressionObject addObject:MGLJSONObjectFromMBGLExpression(child)];
        });
        return expressionObject;
    }
    if (auto stepExpression = dynamic_cast<const Step *>(&mbglExpression)) {
        auto &input = stepExpression->getInput();
        NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"step", MGLJSONObjectFromMBGLExpression(*input.get()), nil];
        stepExpression->eachStop([&](double stop, const Expression &child) {
            [expressionObject addObject:@(stop)];
            [expressionObject addObject:MGLJSONObjectFromMBGLExpression(child)];
        });
        if ([expressionObject[2] isEqual:@(-INFINITY)]) {
            [expressionObject removeObjectAtIndex:2];
        }
        return expressionObject;
    }
    if (auto interpolateExpression = dynamic_cast<const InterpolateBase *>(&mbglExpression)) {
        auto &interpolator = interpolateExpression->getInterpolator();
        auto &input = interpolateExpression->getInput();
        NSArray *interpolatorObject;
        if (interpolator.is<ExponentialInterpolator>()) {
            auto exponentialInterpolator = interpolator.get<ExponentialInterpolator>();
            interpolatorObject = exponentialInterpolator.base == 1 ? @[@"linear"] : @[@"exponential", @(exponentialInterpolator.base)];
        } else if (interpolator.is<CubicBezierInterpolator>()) {
            auto cubicBezierInterpolator = interpolator.get<CubicBezierInterpolator>();
            auto bezier = cubicBezierInterpolator.ub;
            interpolatorObject = @[
                @"cubic-bezier",
                @(bezier.getP1().first), @(bezier.getP1().second),
                @(bezier.getP2().first), @(bezier.getP2().second),
            ];
        } else {
            NSCAssert(NO, @"Unrecognized interpolator type.");
        }
        NSMutableArray *expressionObject = [NSMutableArray arrayWithObjects:@"interpolate", interpolatorObject, MGLJSONObjectFromMBGLExpression(*input.get()), nil];
        interpolateExpression->eachStop([&](double stop, const Expression &child) {
            [expressionObject addObject:@(stop)];
            [expressionObject addObject:MGLJSONObjectFromMBGLExpression(child)];
        });
        return expressionObject;
    }
    if (auto caseExpression = dynamic_cast<const Case *>(&mbglExpression)) {
        NSMutableArray *expressionObject = [NSMutableArray arrayWithObject:@"case"];
        caseExpression->eachChild([&](const Expression &child) {
            [expressionObject addObject:MGLJSONObjectFromMBGLExpression(child)];
        });
        return expressionObject;
    }
    NSCAssert(NO, @"Unrecognized expression type.");
    return nil;
}

