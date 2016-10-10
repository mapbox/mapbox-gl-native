#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@interface MGLStyleValue<T> : NSObject

+ (instancetype)valueWithRawValue:(T)rawValue;
+ (instancetype)valueWithBase:(CGFloat)base stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;
+ (instancetype)valueWithStops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

@end

@interface MGLStyleConstantValue<T> : MGLStyleValue

+ (instancetype)valueWithRawValue:(T)rawValue;

- (instancetype)init NS_UNAVAILABLE;

- (instancetype)initWithRawValue:(T)rawValue NS_DESIGNATED_INITIALIZER;

@property (nonatomic) T rawValue;

@end

@interface MGLStyleFunction<T> : MGLStyleValue

+ (instancetype)functionWithBase:(CGFloat)base stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

+ (instancetype)functionWithStops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

- (instancetype)initWithBase:(CGFloat)base stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops NS_DESIGNATED_INITIALIZER;

@property (nonatomic) CGFloat base;
@property (nonatomic, copy) NSDictionary<NSNumber *, MGLStyleValue<T> *> *stops;

@end
