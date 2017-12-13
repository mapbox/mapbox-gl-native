#import <Foundation/Foundation.h>

#import "NSExpression+MGLAdditions.h"
#include <mbgl/style/filter.hpp>

@interface NSPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter;

+ (instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter;

@end

@interface NSPredicate (MGLExpressionAdditions)

+ (instancetype)mgl_predicateWithJSONObject:(id)object;

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end
