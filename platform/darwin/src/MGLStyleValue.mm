#import "MGLStyleValue_Private.h"

const MGLStyleFunctionOption MGLStyleFunctionOptionInterpolationBase = @"MGLStyleFunctionOptionInterpolationBase";
const MGLStyleFunctionOption MGLStyleFunctionOptionDefaultValue = @"MGLStyleFunctionOptionDefaultValue";

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
