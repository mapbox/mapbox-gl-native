#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface NSExpression (MGLAdditions)

/**
 Returns an expression equivalent to the given Foundation object deserialized
 from JSON data.
 
 The Foundation object is interpreted according to the
 [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions).
 See the “[Predicates and Expressions](../predicates-and-expressions.html)”
 guide for a correspondence of operators and types between the style
 specification and the `NSExpression` representation used by this SDK.
 
 @param object A Foundation object deserialized from JSON data, for example
    using `NSJSONSerialization`.
 @return An initialized expression equivalent to `object`, suitable for use as
    the value of a style layer attribute.
 */
+ (instancetype)mgl_expressionWithJSONObject:(id)object;

/**
 An equivalent Foundation object that can be serialized as JSON.
 
 The Foundation object conforms to the
 [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions).
 See the “[Predicates and Expressions](../predicates-and-expressions.html)”
 guide for a correspondence of operators and types between the style
 specification and the `NSExpression` representation used by this SDK.
 
 You can use `NSJSONSerialization` to serialize the Foundation object as data to
 write to a file.
 */
@property (nonatomic, readonly) id mgl_jsonExpressionObject;

@end

NS_ASSUME_NONNULL_END
