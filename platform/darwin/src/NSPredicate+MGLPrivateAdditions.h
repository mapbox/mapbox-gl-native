#import <Foundation/Foundation.h>

#import "NSPredicate+MGLAdditions.h"

#include <mbgl/style/filter.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface NSPredicate (MGLPrivateAdditions)

- (mbgl::style::Filter)mgl_filter;

+ (nullable instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter;

@end

@interface NSPredicate (MGLExpressionAdditions)

- (nullable id)mgl_if:(id)firstValue, ...;

- (nullable id)mgl_match:(NSExpression *)firstCase, ...;

@end

NS_ASSUME_NONNULL_END
