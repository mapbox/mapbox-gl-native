#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

typedef NSString *MGLExpressionStyleFunction NS_STRING_ENUM;
typedef NSString *MGLExpressionInterpolationMode NS_TYPED_EXTENSIBLE_ENUM;

/**
 An `NSString` identifying the `zoomLevel` operator in an `NSExpression`.
 
 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-zoom"><code>zoom</code></a>
 expression reference in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLExpressionStyleFunction MGLExpressionStyleFunctionZoomLevel;

/**
 An `NSString` identifying the `heatmapDensity` operator in an `NSExpression`.
 
 This attribute corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-heatmap-density"><code>heatmap-density</code></a>
 expression reference in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLExpressionStyleFunction MGLExpressionStyleFunctionHeatmapDensity;

/**
 An `NSString` identifying the `linear` interpolation type in an `NSExpression`.
 
 This attribute corresponds to the `linear` value in the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-interpolate"><code>interpolate</code></a>
 expression reference in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLExpressionInterpolationMode MGLExpressionInterpolationModeLinear;

/**
 An `NSString` identifying the `expotential` interpolation type in an `NSExpression`.
 
 This attribute corresponds to the `exponential` value in the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-interpolate"><code>interpolate</code></a>
 expression reference in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLExpressionInterpolationMode MGLExpressionInterpolationModeExponential;

/**
 An `NSString` identifying the `cubic-bezier` interpolation type in an `NSExpression`.
 
 This attribute corresponds to the `cubic-bezier` value in the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-interpolate"><code>interpolate</code></a>
 expression reference in the Mapbox Style Specification.
 */
extern MGL_EXPORT const MGLExpressionInterpolationMode MGLExpressionInterpolationModeCubicBezier;

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

/**
 Returns a constant expression containing an `NSString`.
 
 This is equivalent to call `[NSExpression expressionForConstantValue:]`.
 
 @param string The string constant.
 */
+ (instancetype)mgl_expressionForString:(nonnull NSString *)string;

/**
 Returns a constant expression containing an `MGLColor`.
 
 This is equivalent to call `[NSExpression expressionForConstantValue:]`.
 
 @param color The color constant.
 */
+ (instancetype)mgl_expressionForColor:(nonnull MGLColor *)color;

/**
 Returns a constant expression containing an `NSValue`.
 
 This is equivalent to call `[NSExpression expressionForConstantValue:]`.
 
 @param value The value constant.
 */
+ (instancetype)mgl_expressionForValue:(nonnull NSValue *)value;

/**
 Returns a conditional function expression specifying the string predicate, and
 colors for each condition.
 
 @param conditionPredicate The predicate to get evaluated.
 @param trueColor The color for conditions equal to true.
 @param falseColor The color value if the condition is equal to false.
 */
+ (instancetype)mgl_expressionForConditional:(nonnull NSString *)conditionPredicate trueColor:(nonnull MGLColor*)trueColor falseColor:(nonnull MGLColor *)falseColor;

/**
 Returns a conditional function expression specifying the string predicate, and
 values for each condition.
 
 @param conditionPredicate The predicate to get evaluated.
 @param trueValue The value for conditions equal to true.
 @param falseValue The value for conditions equal to false.
 */
+ (instancetype)mgl_expressionForConditional:(nonnull NSString *)conditionPredicate trueValue:(nonnull NSValue *)trueValue falseValue:(nonnull NSValue *)falseValue;

/**
 Returns a conditional function expression specifying the string predicate, and
 expressions for each condition.
 
 @param conditionPredicate The predicate to get evaluated.
 @param trueExpression The expression for conditions equal to true.
 @param falseExpression The expression for conditions equal to false.
 */
+ (instancetype)mgl_expressionForConditional:(nonnull NSString *)conditionPredicate trueExpression:(nonnull NSExpression *)trueExpression falseExpresssion:(nonnull NSExpression *)falseExpression;

/**
 Returns a step function expression specifying the function operator, default value
 and stops.
 
 @param function The operator type in which this expression is applied.
 @param value The default value can be boolean or numeric.
 @param stops The stops dictionay must be numeric literals in strictly ascending order.
 */
+ (instancetype)mgl_expressionForStepFunction:(nonnull MGLExpressionStyleFunction)function defaultValue:(nonnull NSValue *)value stops:(nonnull NS_DICTIONARY_OF(NSNumber *, id) *)stops;

/**
 Returns a step function expression specifying the function operator, default color
 and stops.
 
 @param function The operator type in which this expression is applied.
 @param color The default color.
 @param stops The stops dictionay must be numeric literals in strictly ascending order.
 */
+ (instancetype)mgl_expressionForStepFunction:(nonnull MGLExpressionStyleFunction)function defaultColor:(nonnull MGLColor *)color stops:(nonnull NS_DICTIONARY_OF(NSNumber *, id) *)stops;

/**
 Returns a step function expression specifying the function operator, default expression
 and stops.
 
 @param operatorExpression The operator expression.
 @param expression The expression which could be a constant or function expression.
 @param stops The stops dictionay must be numeric literals in strictly ascending order.
 */
+ (instancetype)mgl_expressionForStepFunction:(nonnull NSExpression*)operatorExpression defaultExpression:(nonnull NSExpression *)expression stops:(nonnull NSExpression*)stops;

/**
 Returns an interpolated function expression specifying the function operator, curve type
 and steps.
 
 @param function The operator type in which this expression is applied.
 @param curveType The curve type could be `MGLExpressionInterpolationModeLinear`,
                    `MGLExpressionInterpolationModeExponential` and
                    `MGLExpressionInterpolationModeCubicBezier`.
 @param steps The steps dictionay must be numeric literals in strictly ascending order.
 */
+ (instancetype)mgl_expressionForInterpolateFunction:(nonnull MGLExpressionStyleFunction)function curveType:(nonnull MGLExpressionInterpolationMode)curveType steps:(nonnull NS_DICTIONARY_OF(NSNumber *, id) *)steps;

/**
 Returns an interpolated function expression specifying the function operator, curve type,
 parameters and steps.
 
 @param expressionOperator The expression operator.
 @param curveType The curve type could be `MGLExpressionInterpolationModeLinear`,
 `MGLExpressionInterpolationModeExponential` and
 `MGLExpressionInterpolationModeCubicBezier`.
 @param parameters The parameters expression.
 @param steps The steps expression.
 */
+ (instancetype)mgl_expressionForInterpolateFunction:(nonnull NSExpression*)expressionOperator curveType:(nonnull MGLExpressionInterpolationMode)curveType parameters:(nullable NSExpression *)parameters steps:(nonnull NSExpression*)steps;

/**
 Returns a string constant expression appending the passed string.
 
 @param string The string to append.
 */
- (instancetype)mgl_appendingString:(NSString *)string;

/**
 Returns a string constant expression appending the passed expression.
 
 @param expression The evaluated expression must return a string.
 */
- (instancetype)mgl_appendingExpression:(NSExpression *)expression;


@end

NS_ASSUME_NONNULL_END
