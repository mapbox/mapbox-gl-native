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
 
 You can use `NSJSONSerialization` to serialize the Foundation object as data to
 write to a file.
 */
@property (nonatomic, readonly) id mgl_jsonExpressionObject;

/**
 Returns a copy of the receiver localized into the given locale.
 
 This method assumes the receiver is used with a style layer property such as
 `MGLStyleLayer.text` and refers to the feature attributes that are available in
 vector tiles supplied by the
 <a href="https://www.mapbox.com/vector-tiles/mapbox-streets-v7/#overview">Mapbox Streets source</a>.
 On iOS, the user can set the system’s preferred language in Settings, General
 Settings, Language & Region. On macOS, the user can set the system’s preferred
 language in the Language & Region pane of System Preferences.
 
 @param locale The locale into which labels should be localized. To use the
    system’s preferred language, if supported, specify `nil`. To use the local
    language, specify a locale with the identifier `mul`.
 @param replacesTokens `YES` to localize `{token}` syntax in constant value
    subexpressions; `NO` otherwise.
 */
- (NSExpression *)mgl_expressionLocalizedIntoLocale:(nullable NSLocale *)locale replacingTokens:(BOOL)replacesTokens;

@end

NS_ASSUME_NONNULL_END
