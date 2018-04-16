#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#else
    #import <Cocoa/Cocoa.h>
#endif

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

typedef NSString *MGLExpressionInterpolationMode NS_TYPED_EXTENSIBLE_ENUM;

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

@interface NSExpression (MGLVariableAdditions)

/**
 `NSExpression` variable that corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-zoom"><code>zoom</code></a>
 expression reference in the Mapbox Style Specification.
 */
@property (class, nonatomic, readonly) NSExpression *zoomLevelVariableExpression;

/**
 `NSExpression` variable that corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-heatmap-density"><code>heatmap-density</code></a>
 expression reference in the Mapbox Style Specification.
 */
@property (class, nonatomic, readonly) NSExpression *heatmapDensityVariableExpression;

/**
 `NSExpression` variable that corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#eexpressions-geometry-type"><code>geometry-type</code></a>
 expression reference in the Mapbox Style Specification.
 */
@property (class, nonatomic, readonly) NSExpression *geometryTypeVariableExpression;

/**
 `NSExpression` variable that corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-id"><code>id</code></a>
 expression reference in the Mapbox Style Specification.
 */
@property (class, nonatomic, readonly) NSExpression *featureIdentifierVariableExpression;

/**
 `NSExpression` variable that corresponds to the
 <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions-properties"><code>properties</code></a>
 expression reference in the Mapbox Style Specification.
 */
@property (class, nonatomic, readonly) NSExpression *featurePropertiesVariableExpression;

@end

@interface NSExpression (MGLInitializerAdditions)

/**
 Returns a conditional function expression specifying the string predicate, and
 expressions for each condition.
 
 @param conditionPredicate The predicate to get evaluated.
 @param trueExpression The expression for conditions equal to true.
 @param falseExpression The expression for conditions equal to false.
 */
+ (instancetype)mgl_expressionForConditional:(nonnull NSPredicate *)conditionPredicate trueExpression:(nonnull NSExpression *)trueExpression falseExpresssion:(nonnull NSExpression *)falseExpression NS_SWIFT_NAME(init(forMGLConditional:trueExpression:falseExpression:));

/**
 Returns a step function expression specifying the stepping, from expression
 and stops.
 
 @param steppingExpression The stepping expression.
 @param fromExpression The expression which could be a constant or function expression.
 @param stops The stops must be an `NSDictionary` constant `NSExpression`.
 The wrapped dictionary must be numeric expression literals in strictly ascending order.
 */
+ (instancetype)mgl_expressionForSteppingExpression:(nonnull NSExpression*)steppingExpression fromExpression:(nonnull NSExpression *)fromExpression stops:(nonnull NSExpression*)stops NS_SWIFT_NAME(init(forMGLStepping:from:stops:));

/**
 Returns an interpolated function expression specifying the function operator, curve type,
 parameters and steps.
 
 @param interpolatingExpression The interpolating expression input.
 @param curveType The curve type could be `MGLExpressionInterpolationModeLinear`,
 `MGLExpressionInterpolationModeExponential` and
 `MGLExpressionInterpolationModeCubicBezier`.
 @param parameters The parameters expression.
 @param stops The stops expression.
 */
+ (instancetype)mgl_expressionForInterpolatingExpression:(nonnull NSExpression*)interpolatingExpression withCurveType:(nonnull MGLExpressionInterpolationMode)curveType parameters:(nullable NSExpression *)parameters stops:(nonnull NSExpression*)stops NS_SWIFT_NAME(init(forMGLInterpolating:curveType:parameters:stops:));

/**
 Returns a match function expression specifying the input, matching values,
 and default value.
 
 @param matchingExpression The matching expression.
 @param matchedExpressions The matched values expression dictionary must be condition : value.
 @param defaultExpression The defaultValue expression to be used in case there is no match.
 */
+ (instancetype)mgl_expressionForMatchingExpression:(nonnull NSExpression *)matchingExpression inDictionary:(nonnull NSDictionary<NSExpression *, NSExpression *> *)matchedExpressions defaultExpression:(nonnull NSExpression *)defaultExpression NS_SWIFT_NAME(init(forMGLMatchingKey:matched:default:));
/**
 Returns a constant expression appending the passed expression.
 
 @note Both the receiver and the given expression must be an `NSString` constant
 expression type; otherwise, an exception is rised.
 
 @param expression The expression to append to the receiver.
    system’s preferred language, if supported, specify `nil`. To use the local
    language, specify a locale with the identifier `mul`.
 */
- (instancetype)mgl_expressionByAppendingExpression:(nonnull NSExpression *)expression NS_SWIFT_NAME(mgl_appending(_:));

@end

@interface NSExpression (MGLAdditions)

/**
 Returns an expression equivalent to the given Foundation object deserialized
 from JSON data.
 
 The Foundation object is interpreted according to the
 [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions).
 See the
 “[Information for Style Authors](../for-style-authors.html#setting-attribute-values)”
 guide for a correspondence of operators and types between the style
 specification and the `NSExpression` representation used by this SDK.
 
 @param object A Foundation object deserialized from JSON data, for example
    using `NSJSONSerialization`.
 @return An initialized expression equivalent to `object`, suitable for use as
    the value of a style layer attribute.
 */
+ (instancetype)expressionWithMGLJSONObject:(id)object NS_SWIFT_NAME(init(mglJSONObject:));

/**
 An equivalent Foundation object that can be serialized as JSON.
 
 The Foundation object conforms to the
 [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-js/style-spec/#expressions).
 See the
 “[Information for Style Authors](../for-style-authors.html#setting-attribute-values)”
 guide for a correspondence of operators and types between the style
 specification and the `NSExpression` representation used by this SDK.
 
 @param locale The locale into which labels should be localized. To use the
 You can use `NSJSONSerialization` to serialize the Foundation object as data to
 write to a file.
 */
@property (nonatomic, readonly) id mgl_jsonExpressionObject;

/**
 Returns a copy of the receiver localized into the given locale.
 
 This method assumes the receiver refers to the feature attributes that are
 available in vector tiles supplied by the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets-v7/#overview">Mapbox Streets source</a>.
 On iOS, the user can set the system’s preferred language in Settings, General
 Settings, Language & Region. On macOS, the user can set the system’s preferred
 language in the Language & Region pane of System Preferences.
 
 @param locale The locale into which labels should be localized. To use the
    system’s preferred language, if supported, specify `nil`. To use the local
    language, specify a locale with the identifier `mul`.
 */
- (NSExpression *)mgl_expressionLocalizedIntoLocale:(nullable NSLocale *)locale;

@end

NS_ASSUME_NONNULL_END
