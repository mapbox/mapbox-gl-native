#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

typedef NSString * MGLAttributedExpressionKey NS_EXTENSIBLE_STRING_ENUM;

FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontNamesAttribute;
FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontSizeAttribute;
FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontColorAttribute;

/**
 An `MGLAttributedExpression` object associates text formatting attibutes (such as font size or
 font names) to an `NSExpression`.
 
 ### Example
 ```swift
 let redColor = UIColor.red
 let expression = NSExpression(forConstantValue: "Foo")
 let attributes: Dictionary<MGLAttributedExpressionKey, Any> = [.fontNamesAttribute : ["DIN Offc Pro Italic",
                                                                                       "Arial Unicode MS Regular"],
                                                                .fontSizeAttribute: 1.2,
                                                                .fontColorAttribute: redColor]
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
 `MGLFontNamesAttribute` | `NSArray<NSString *>*`
 `MGLFontSizeAttribute` | `NSNumber`
 `MGLFontColorAttribute` | `UIColor`

 */
@property (strong, nonatomic, readonly) NSDictionary<MGLAttributedExpressionKey, id> *attributes;
#else
/**
 The formatting attributes dictionary.
 Key | Value Type
 --- | ---
 `MGLFontNamesAttribute` | `NSArray<NSString *>*`
 `MGLFontSizeAttribute` | `NSNumber`
 `MGLFontColorAttribute` | `NSColor`
 */
@property (strong, nonatomic, readonly) NSDictionary<MGLAttributedExpressionKey, id> *attributes;
#endif


/**
 Returns an `MGLAttributedExpression` object initialized with an expression and no attribute information.
 */
- (instancetype)initWithExpression:(NSExpression *)expression;

/** 
 Returns an `MGLAttributedExpression` object initialized with an expression and text format attributes.
 */
- (instancetype)initWithExpression:(NSExpression *)expression attributes:(nonnull NSDictionary <MGLAttributedExpressionKey, id> *)attrs;

/**
 Creates an `MGLAttributedExpression` object initialized with an expression and the format attributes for font names and font size.
 */
+ (instancetype)attributedExpression:(NSExpression *)expression fontNames:(nullable NSArray<NSString*> *)fontNames fontSize:(nullable NSNumber *)fontSize;

/**
 Creates an `MGLAttributedExpression` object initialized with an expression and the format attributes dictionary.
 */
+ (instancetype)attributedExpression:(NSExpression *)expression attributes:(nonnull NSDictionary <MGLAttributedExpressionKey, id> *)attrs;

@end

NS_ASSUME_NONNULL_END
