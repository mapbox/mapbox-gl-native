#import "MGLStyleValue_Private.h"

@implementation MGLStyleValue

+ (instancetype)valueWithRawValue:(id)rawValue {
    return [MGLStyleConstantValue valueWithRawValue:rawValue];
}

+ (instancetype)valueWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    return [MGLStyleFunction functionWithInterpolationBase:interpolationBase stops:stops];
}

+ (instancetype)valueWithStops:(NSDictionary *)stops {
    return [MGLStyleFunction functionWithStops:stops];
}

@end

@implementation MGLStyleConstantValue

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

- (BOOL)isEqual:(MGLStyleConstantValue *)other {
    return [other isKindOfClass:[self class]] && [other.rawValue isEqual:self.rawValue];
}

- (NSUInteger)hash {
    return [self.rawValue hash];
}

@end

@implementation MGLStyleFunction

+ (instancetype)functionWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    return [[self alloc] initWithInterpolationBase:interpolationBase stops:stops];
}

+ (instancetype)functionWithStops:(NSDictionary *)stops {
    return [[self alloc] initWithInterpolationBase:1 stops:stops];
}

- (instancetype)init {
    return [self initWithInterpolationBase:1 stops:@{}];
}

- (instancetype)initWithInterpolationBase:(CGFloat)interpolationBase stops:(NSDictionary *)stops {
    if (self = [super init]) {
        if (!stops.count)
        {
            [NSException raise:NSInvalidArgumentException format:@"%@ requires at least one stop.", self];
        }
        _interpolationBase = interpolationBase;
        _stops = stops;
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p, interpolationBase = %f; stops = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.interpolationBase,
            self.stops];
}

- (BOOL)isEqual:(MGLStyleFunction *)other {
    return ([other isKindOfClass:[self class]] && other.interpolationBase == self.interpolationBase
            && [other.stops isEqualToDictionary:self.stops]);
}

- (NSUInteger)hash {
    return self.interpolationBase + self.stops.hash;
}

@end
