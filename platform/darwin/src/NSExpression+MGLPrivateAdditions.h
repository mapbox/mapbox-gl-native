#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#import "NSExpression+MGLAdditions.h"

#include <mbgl/style/filter.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface NSObject (MGLExpressionAdditions)

- (NSNumber *)mgl_number;
- (NSNumber *)mgl_numberWithFallbackValues:(id)fallbackValue, ... NS_REQUIRES_NIL_TERMINATION;

@end

@interface NSExpression (MGLPrivateAdditions)

@property (nonatomic, readonly) mbgl::Value mgl_constantMBGLValue;
@property (nonatomic, readonly) std::vector<mbgl::Value> mgl_aggregateMBGLValue;
@property (nonatomic, readonly) mbgl::FeatureType mgl_featureType;
@property (nonatomic, readonly) std::vector<mbgl::FeatureType> mgl_aggregateFeatureType;
@property (nonatomic, readonly) mbgl::FeatureIdentifier mgl_featureIdentifier;
@property (nonatomic, readonly) std::vector<mbgl::FeatureIdentifier> mgl_aggregateFeatureIdentifier;

@end

@interface NSNull (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSString (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSNumber (MGLExpressionAdditions)

- (id)mgl_interpolateWithCurveType:(NSString *)curveType parameters:(NSArray *)parameters stops:(NSDictionary<NSNumber *, id> *)stops;
- (id)mgl_stepWithMinimum:(id)minimum stops:(NSDictionary<NSNumber *, id> *)stops;

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSArray (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSDictionary (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface MGLColor (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSExpression (MGLExpressionAdditions)

- (NSExpression *)mgl_expressionWithContext:(NSDictionary<NSString *, NSExpression *> *)context;

@end

extern NSArray *MGLSubexpressionsWithJSONObjects(NSArray *objects);

NS_ASSUME_NONNULL_END
