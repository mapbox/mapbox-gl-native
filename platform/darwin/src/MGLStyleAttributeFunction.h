#import <Foundation/Foundation.h>

#import "MGLTypes.h"
#import "MGLStyleAttributeValue.h"


NS_ASSUME_NONNULL_BEGIN

@interface MGLStyleAttributeFunction : NSObject <MGLStyleAttributeValue>

@property (nonatomic, copy) NS_DICTIONARY_OF(NSNumber *, id) *stops;

@property (nonatomic, copy, nullable) NSNumber *base;

@end

@interface MGLStyleValue<T> : NSObject

+ (instancetype)valueWithRawValue:(T)rawValue;
+ (instancetype)valueWithBase:(CGFloat)base stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;
+ (instancetype)valueWithStops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

@end

@interface MGLStyleConstantValue<T> : MGLStyleValue

+ (instancetype)valueWithRawValue:(T)rawValue;

- (instancetype)initWithRawValue:(T)rawValue;

@property (nonatomic) T rawValue;

@end

@interface MGLStyleFunction<T> : MGLStyleValue

+ (instancetype)functionWithBase:(CGFloat)base stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;
+ (instancetype)functionWithStops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

- (instancetype)initWithBase:(CGFloat)base stops:(NSDictionary<NSNumber *, MGLStyleValue<T> *> *)stops;

@property (nonatomic) CGFloat base;
@property (nonatomic, copy) NSDictionary<NSNumber *, MGLStyleValue<T> *> *stops;

@end

NS_ASSUME_NONNULL_END
