#import <Foundation/Foundation.h>

#include <mbgl/style/filter.hpp>

NS_ASSUME_NONNULL_BEGIN

@interface NSExpression (MGLAdditions)

@property (nonatomic, readonly) mbgl::Value mgl_constantMBGLValue;
@property (nonatomic, readonly) std::vector<mbgl::Value> mgl_aggregateMBGLValue;
@property (nonatomic, readonly) mbgl::FeatureType mgl_featureType;
@property (nonatomic, readonly) std::vector<mbgl::FeatureType> mgl_aggregateFeatureType;
@property (nonatomic, readonly) mbgl::FeatureIdentifier mgl_featureIdentifier;
@property (nonatomic, readonly) std::vector<mbgl::FeatureIdentifier> mgl_aggregateFeatureIdentifier;

@end

@interface NSExpression (MGLExpressionAdditions)

+ (instancetype)mgl_expressionWithJSONObject:(id)object;

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSString (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSNumber (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

@interface NSArray (MGLExpressionAdditions)

@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

extern NSArray *MGLSubexpressionsWithJSONObjects(NSArray *objects);

NS_ASSUME_NONNULL_END
