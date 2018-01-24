#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

typedef NSString *MGLStyleFunctionOption NS_STRING_ENUM NS_UNAVAILABLE;

extern MGL_EXPORT const MGLStyleFunctionOption MGLStyleFunctionOptionInterpolationBase __attribute__((unavailable("Use NSExpression instead, applying the mgl_interpolateWithCurveType:parameters:stops: function with a curve type of “exponential” and a non-nil parameter.")));

extern MGL_EXPORT const MGLStyleFunctionOption MGLStyleFunctionOptionDefaultValue __attribute__((unavailable("Use +[NSExpression expressionForConditional:trueExpression:falseExpression:] instead.")));

typedef NS_ENUM(NSUInteger, MGLInterpolationMode) {
    MGLInterpolationModeExponential __attribute__((unavailable("Use NSExpression instead, applying the mgl_interpolateWithCurveType:parameters:stops: function with a curve type of “exponential”."))) = 0,
    MGLInterpolationModeInterval __attribute__((unavailable("Use NSExpression instead, calling the mgl_stepWithMinimum:stops: function."))),
    MGLInterpolationModeCategorical __attribute__((unavailable("Use NSExpression instead."))),
    MGLInterpolationModeIdentity __attribute__((unavailable("Use +[NSExpression expressionForKeyPath:] instead.")))
} __attribute__((unavailable("Use NSExpression instead.")));

MGL_EXPORT __attribute__((unavailable("Use NSExpression instead.")))
@interface MGLStyleValue<T> : NSObject
@end

MGL_EXPORT __attribute__((unavailable("Use +[NSExpression expressionForConstantValue:] instead.")))
@interface MGLConstantStyleValue<T> : MGLStyleValue<T>
@end

@compatibility_alias MGLStyleConstantValue MGLConstantStyleValue;

MGL_EXPORT __attribute__((unavailable("Use NSExpression instead, calling the mgl_stepWithMinimum:stops: or mgl_interpolateWithCurveType:parameters:stops: function.")))
@interface MGLStyleFunction<T> : MGLStyleValue<T>
@end

MGL_EXPORT __attribute__((unavailable("Use NSExpression instead, applying the mgl_stepWithMinimum:stops: or mgl_interpolateWithCurveType:parameters:stops: function to the $zoomLevel variable.")))
@interface MGLCameraStyleFunction<T> : MGLStyleFunction<T>
@end

MGL_EXPORT __attribute__((unavailable("Use NSExpression instead, applying the mgl_stepWithMinimum:stops: or mgl_interpolateWithCurveType:parameters:stops: function to a key path expression.")))
@interface MGLSourceStyleFunction<T> : MGLStyleFunction<T>
@end

MGL_EXPORT __attribute__((unavailable("Use a NSExpression instead with nested mgl_stepWithMinimum:stops: or mgl_interpolateWithCurveType:parameters:stops: function calls.")))
@interface MGLCompositeStyleFunction<T> : MGLStyleFunction<T>
@end

NS_ASSUME_NONNULL_END
