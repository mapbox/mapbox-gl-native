#import "MGLStyleValue_Private.h"

@implementation MGLStyleValue

+ (instancetype)valueWithRawValue:(id)rawValue {
    return [MGLStyleConstantValue valueWithRawValue:rawValue];
}

+ (instancetype)valueWithBase:(CGFloat)base stops:(NSDictionary *)stops {
    return [MGLStyleFunction functionWithBase:base stops:stops];
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

+ (instancetype)functionWithBase:(CGFloat)base stops:(NSDictionary *)stops {
    return [[self alloc] initWithBase:base stops:stops];
}

+ (instancetype)functionWithStops:(NSDictionary *)stops {
    return [[self alloc] initWithBase:1 stops:stops];
}

- (instancetype)init {
    return [self initWithBase:1 stops:@{}];
}

- (instancetype)initWithBase:(CGFloat)base stops:(NSDictionary *)stops {
    if (self = [super init]) {
        _base = base;
        _stops = stops;
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%@: %p, base = %f; stops = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.base,
            self.stops];
}

- (BOOL)isEqual:(MGLStyleFunction *)other {
    return ([other isKindOfClass:[self class]] && other.base == self.base
            && [other.stops isEqualToDictionary:self.stops]);
}

- (NSUInteger)hash {
    return self.base + self.stops.hash;
}

@end
