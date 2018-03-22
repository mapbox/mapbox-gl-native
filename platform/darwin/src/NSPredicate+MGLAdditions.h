#import <Foundation/Foundation.h>

#import "NSExpression+MGLPrivateAdditions.h"

@interface NSPredicate (MGLAdditions)

- (mbgl::style::Filter)mgl_filter;

+ (instancetype)mgl_predicateWithFilter:(mbgl::style::Filter)filter;

@end

@interface NSPredicate (MGLExpressionAdditions)

+ (instancetype)mgl_predicateWithJSONObject:(id)object;

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

- (id)mgl_case:(id)firstValue, ...;

- (id)mgl_matchWithOptions:(NSArray *)options default:(id)minimum;

@end
