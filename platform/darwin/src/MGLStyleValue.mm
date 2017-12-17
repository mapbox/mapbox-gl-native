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

@implementation MGLStyleValue

+ (instancetype)valueWithRawValue:(id)rawValue {
    return [MGLConstantStyleValue valueWithRawValue:rawValue];
}

+ (instancetype)valueWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    return [MGLCameraStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential stops:stops options:@{MGLStyleFunctionOptionInterpolationBase: @(interpolationBase)}];
}

+ (instancetype)valueWithStops:(NSDictionary *)stops {
    return [MGLCameraStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential stops:stops options:nil];
}

+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode cameraStops:(NSDictionary *)cameraStops options:(NSDictionary *)options {
    return [MGLCameraStyleFunction functionWithInterpolationMode:interpolationMode stops:cameraStops options:options];
}

+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode sourceStops:(NSDictionary *)sourceStops attributeName:(NSString *)attributeName options:(NSDictionary *)options {
    return [MGLSourceStyleFunction functionWithInterpolationMode:interpolationMode stops:sourceStops attributeName:attributeName options:options];
}

+ (instancetype)valueWithInterpolationMode:(MGLInterpolationMode)interpolationMode compositeStops:(NSDictionary *)compositeStops attributeName:(NSString *)attributeName options:(NSDictionary *)options {
    return [MGLCompositeStyleFunction functionWithInterpolationMode:interpolationMode stops:compositeStops attributeName:attributeName options:options];
}

@end

@implementation MGLConstantStyleValue

+ (instancetype)valueWithRawValue:(id)rawValue {
    return [[self alloc] initWithRawValue:rawValue];
}

- (instancetype)initWithRawValue:(id)rawValue {
    if (self = [super init]) {
        _rawValue = rawValue;
    }
    return self;
}

- (NSString *)description {
    return [self.rawValue description];
}

- (NSString *)debugDescription {
    return [self.rawValue debugDescription];
}

- (BOOL)isEqual:(MGLConstantStyleValue *)other {
    return [other isKindOfClass:[self class]] && [other.rawValue isEqual:self.rawValue];
}

- (NSUInteger)hash {
    return [self.rawValue hash];
}

@end

@implementation MGLStyleFunction

+ (instancetype)functionWithStops:(NSDictionary *)stops {
    return [MGLCameraStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential stops:stops options:nil];
}

+ (instancetype)functionWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    return [MGLCameraStyleFunction functionWithInterpolationMode:MGLInterpolationModeExponential stops:stops options:@{MGLStyleFunctionOptionInterpolationBase: @(interpolationBase)}];
}

- (instancetype)init {
    if (self = [super init]) {
        self.interpolationBase = 1.0;
        self.stops = @{};
    }
    return self;
}

- (instancetype)initWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    if (self = [super init]) {
        self.interpolationBase = interpolationBase;
        self.stops = stops;
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p, \
            stops = %@, \
            interpolationBase = %f>",
            NSStringFromClass([self class]), (void *)self,
            self.stops,
            self.interpolationBase];
}

- (BOOL)isEqual:(MGLStyleFunction *)other {
    return ([other isKindOfClass:[self class]]
            && [other.stops isEqualToDictionary:self.stops]
            && other.interpolationBase == self.interpolationBase);
}

- (NSUInteger)hash {
    return  self.stops.hash + @(self.interpolationBase).hash;
}

@end

@implementation MGLCameraStyleFunction

@dynamic stops;

+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NSDictionary *)stops options:(NSDictionary *)options {
    return [[self alloc] initWithInterpolationMode:interpolationMode stops:stops options:options];
}

- (instancetype)initWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    return [self initWithInterpolationMode:MGLInterpolationModeExponential stops:stops options:@{MGLStyleFunctionOptionInterpolationBase: @(interpolationBase)}];
}

- (instancetype)initWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NSDictionary *)stops options:(NSDictionary *)options {
    if (![stops count]) {
        [NSException raise:NSInvalidArgumentException
                    format:@"Camera functions must have at least one stop."];
        return {};
    }

    if (self = [super init]) {
        self.interpolationMode = interpolationMode;
        self.stops = stops;

        if ([options.allKeys containsObject:MGLStyleFunctionOptionInterpolationBase]) {
            if ([options[MGLStyleFunctionOptionInterpolationBase] isKindOfClass:[NSNumber class]]) {
                NSNumber *value = (NSNumber *)options[MGLStyleFunctionOptionInterpolationBase];
                self.interpolationBase = [value floatValue];
            } else {
                [NSException raise:NSInvalidArgumentException format:@"Interpolation base must be an NSNumber that represents a CGFloat."];
            }
        }
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p, \
            interpolationMode = %lu, \
            stops = %@, \
            interpolationBase = %f>",
            NSStringFromClass([self class]), (void *)self,
            (unsigned long)self.interpolationMode,
            self.stops,
            self.interpolationBase];
}

- (BOOL)isEqual:(MGLCameraStyleFunction *)other {
    return ([other isKindOfClass:[self class]]
            && other.interpolationMode == self.interpolationMode
            && [other.stops isEqualToDictionary:self.stops]
            && other.interpolationBase == self.interpolationBase);
}

- (NSUInteger)hash {
    return  @(self.interpolationMode).hash + self.stops.hash + @(self.interpolationBase).hash;
}

@end

@implementation MGLSourceStyleFunction

@dynamic stops;

+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NSDictionary *)stops attributeName:(NSString *)attributeName options:(NSDictionary *)options {
    return [[self alloc] initWithInterpolationMode:interpolationMode stops:stops attributeName:attributeName options:options];
}

- (instancetype)initWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    return [self initWithInterpolationMode:MGLInterpolationModeExponential stops:stops attributeName:@"" options:@{MGLStyleFunctionOptionInterpolationBase: @(interpolationBase)}];
}

- (instancetype)initWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NSDictionary *)stops attributeName:(NSString *)attributeName options:(NSDictionary *)options {
    if (self = [super init]) {
        self.interpolationMode = interpolationMode;
        self.stops = stops;
        _attributeName = attributeName;

        if ([options.allKeys containsObject:MGLStyleFunctionOptionDefaultValue]) {
            if ([options[MGLStyleFunctionOptionDefaultValue] isKindOfClass:[MGLStyleValue class]]) {
                MGLStyleValue *value = (MGLStyleValue *)options[MGLStyleFunctionOptionDefaultValue];
                _defaultValue = value;
            } else {
                [NSException raise:NSInvalidArgumentException format:@"Default value must be an MGLStyleValue"];
            }
        }

        if ([options.allKeys containsObject:MGLStyleFunctionOptionInterpolationBase]) {
            if ([options[MGLStyleFunctionOptionInterpolationBase] isKindOfClass:[NSNumber class]]) {
                NSNumber *value = (NSNumber *)options[MGLStyleFunctionOptionInterpolationBase];
                self.interpolationBase = [value floatValue];
            } else {
                [NSException raise:NSInvalidArgumentException format:@"Interpolation base must be an NSNumber that represents a CGFloat."];
            }
        }
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p, \
            interpolationMode = %lu, \
            stops = %@, \
            attributeName = %@, \
            defaultValue = %@, \
            interpolationBase = %f>",
            NSStringFromClass([self class]),
            (void *)self,
            (unsigned long)self.interpolationMode,
            self.stops,
            self.attributeName,
            self.defaultValue,
            self.interpolationBase];
}

- (BOOL)isEqual:(MGLSourceStyleFunction *)other {
    return ([other isKindOfClass:[self class]]
            && other.interpolationMode == self.interpolationMode
            && ((self.stops && [other.stops isEqualToDictionary:self.stops]) || (!self.stops && !other.stops))
            && [other.attributeName isEqual:self.attributeName]
            && ((self.defaultValue && [other.defaultValue isEqual:self.defaultValue]) || (!self.defaultValue && !other.defaultValue))
            && other.interpolationBase == self.interpolationBase);
}

- (NSUInteger)hash {
    return @(self.interpolationMode).hash + self.stops.hash + self.attributeName.hash + self.defaultValue.hash + @(self.interpolationBase).hash;
}

@end

@implementation MGLCompositeStyleFunction

@dynamic stops;

+ (instancetype)functionWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NSDictionary *)stops attributeName:(NSString *)attributeName options:(NSDictionary *)options {
    return [[self alloc] initWithInterpolationMode:interpolationMode stops:stops attributeName:attributeName options:options];
}

- (instancetype)initWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    return [self initWithInterpolationMode:MGLInterpolationModeExponential stops:stops attributeName:@"" options:@{MGLStyleFunctionOptionInterpolationBase: @(interpolationBase)}];
}

- (instancetype)initWithInterpolationMode:(MGLInterpolationMode)interpolationMode stops:(NSDictionary *)stops attributeName:(NSString *)attributeName options:(NSDictionary *)options {
    if (self = [super init]) {
        self.interpolationMode = interpolationMode;
        self.stops = stops;
        _attributeName = attributeName;

        if ([options.allKeys containsObject:MGLStyleFunctionOptionDefaultValue]) {
            if ([options[MGLStyleFunctionOptionDefaultValue] isKindOfClass:[MGLStyleValue class]]) {
                MGLStyleValue *value = (MGLStyleValue *)options[MGLStyleFunctionOptionDefaultValue];
                _defaultValue = value;
            } else {
                [NSException raise:NSInvalidArgumentException format:@"Default value must be an MGLStyleValue"];
            }
        }

        if ([options.allKeys containsObject:MGLStyleFunctionOptionInterpolationBase]) {
            if ([options[MGLStyleFunctionOptionInterpolationBase] isKindOfClass:[NSNumber class]]) {
                NSNumber *value = (NSNumber *)options[MGLStyleFunctionOptionInterpolationBase];
                self.interpolationBase = [value floatValue];
            } else {
                [NSException raise:NSInvalidArgumentException format:@"Interpolation base must be an NSNumber that represents a CGFloat."];
            }
        }
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p, \
            interpolationMode = %lu, \
            stops = %@, \
            attributeName = %@, \
            defaultValue = %@, \
            interpolationBase = %f>",
            NSStringFromClass([self class]), (void *)self,
            (unsigned long)self.interpolationMode,
            self.stops,
            self.attributeName,
            self.defaultValue,
            self.interpolationBase];
}

- (BOOL)isEqual:(MGLCompositeStyleFunction *)other {
    return ([other isKindOfClass:[self class]]
            && other.interpolationMode == self.interpolationMode
            && [other.stops isEqualToDictionary:self.stops]
            && [other.attributeName isEqual:self.attributeName]
            && ((self.defaultValue && [other.defaultValue isEqual:self.defaultValue]) || (!self.defaultValue && !other.defaultValue))
            && other.interpolationBase == self.interpolationBase);
}

- (NSUInteger)hash {
    return  @(self.interpolationMode).hash + self.stops.hash + self.attributeName.hash + @(self.interpolationBase).hash;
}

@end
