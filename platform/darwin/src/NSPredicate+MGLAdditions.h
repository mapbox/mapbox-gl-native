#import <Foundation/Foundation.h>

#import "NSExpression+MGLAdditions.h"
#include <mbgl/style/filter.hpp>

@interface NSPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter;

+ (instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter;

@end

