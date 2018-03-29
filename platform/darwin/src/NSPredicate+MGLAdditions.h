#import <Foundation/Foundation.h>

#import "NSExpression+MGLPrivateAdditions.h"

@interface NSPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter;

+ (instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter;

@end

@interface NSPredicate (MGLExpressionAdditions)

+ (instancetype)mgl_predicateWithJSONObject:(id)object;

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

- (id)mgl_if:(id)firstValue, ...;

- (id)mgl_match:(NSExpression *)firstCase, ...;

@end
