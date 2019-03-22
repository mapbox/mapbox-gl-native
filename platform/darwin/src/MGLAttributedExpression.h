#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

typedef NSString * MGLAttributedExpressionKey NS_EXTENSIBLE_STRING_ENUM;

FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontNamesAttribute;
FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontScaleAttribute;
FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontColorAttribute;

/**
 An `MGLAttributedExpression` object associates text formatting attibutes (such as font size or
 font names) to an `NSExpression`.
 
 ### Example
 ```swift
 let redColor = UIColor.red
 let expression = NSExpression(forConstantValue: "Foo")
 let attributes: Dictionary<MGLAttributedExpressionKey, NSExpression> = [.fontNamesAttribute : NSExpression(forConstantValue: ["DIN Offc Pro Italic",
                                                                                                                               "Arial Unicode MS Regular"]),
                                                                         .fontScaleAttribute: NSExpression(forConstantValue: 1.2),
                                                                         .fontColorAttribute: NSExpression(forConstantValue: redColor)]
 let attributedExpression = MGLAttributedExpression(expression, attributes:attributes)
 ```
 
 */
MGL_EXPORT
@interface MGLAttributedExpression : NSObject

/**
 The expression content of the receiver as `NSExpression`.
 */
@property (strong, nonatomic) NSExpression *expression;

#if TARGET_OS_IPHONE
/**
 The formatting attributes dictionary.
 Key | Value Type
 --- | ---
 `MGLFontNamesAttribute` | `NSExpression`
 `MGLFontScaleAttribute` | `NSExpression`
 `MGLFontColorAttribute` | `NSExpression`

 */
@property (strong, nonatomic, readonly) NSDictionary<MGLAttributedExpressionKey, NSExpression *> *attributes;
#else
/**
 The formatting attributes dictionary.
 Key | Value Type
 --- | ---
 `MGLFontNamesAttribute` | `NSExpression`
 `MGLFontScaleAttribute` | `NSExpression`
 `MGLFontColorAttribute` | `NSExpression`
 */
@property (strong, nonatomic, readonly) NSDictionary<MGLAttributedExpressionKey, NSExpression *> *attributes;
#endif


/**
 Returns an `MGLAttributedExpression` object initialized with an expression and no attribute information.
 */
- (instancetype)initWithExpression:(NSExpression *)expression;

/** 
 Returns an `MGLAttributedExpression` object initialized with an expression and text format attributes.
 */
- (instancetype)initWithExpression:(NSExpression *)expression attributes:(nonnull NSDictionary <MGLAttributedExpressionKey, NSExpression *> *)attrs;

/**
 Creates an `MGLAttributedExpression` object initialized with an expression and the format attributes for font names and font size.
 */
+ (instancetype)attributedExpression:(NSExpression *)expression fontNames:(nullable NSArray<NSString*> *)fontNames fontScale:(nullable NSNumber *)fontScale;

/**
 Creates an `MGLAttributedExpression` object initialized with an expression and the format attributes dictionary.
 */
+ (instancetype)attributedExpression:(NSExpression *)expression attributes:(nonnull NSDictionary <MGLAttributedExpressionKey, NSExpression *> *)attrs;

@end

NS_ASSUME_NONNULL_END
