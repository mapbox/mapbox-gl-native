#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

typedef NSString * MGLAttributedExpressionKey NS_EXTENSIBLE_STRING_ENUM;

FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontNamesAttribute;
FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontSizeAttribute;

/**
 An `MGLAttributedExpression` object associates text formatting attibutes (such as font size or
 font names) to an `NSExpression`.
 */
MGL_EXPORT
@interface MGLAttributedExpression : NSObject

/**
 The expression content of the receiver as `NSExpression`.
 */
@property (strong, nonatomic) NSExpression *expression;

/**
 The formatting attributes.
 */
@property (strong, nonatomic, readonly) NSDictionary<MGLAttributedExpressionKey, id> *attributes;

/**
 Returns an `MGLAttributedExpression` object initialized with an expression and no attribute information.
 */
- (instancetype)initWithExpression:(NSExpression *)expression;

/** 
 Returns an `MGLAttributedExpression` object initialized with an expression and text format attributes.
 */
- (instancetype)initWithExpression:(NSExpression *)expression attributes:(nullable NSDictionary <MGLAttributedExpressionKey, id> *)attrs;

/**
 Creates an `MGLAttributedExpression` object initialized with an expression and the format attributes for font names and font size.
 */
+ (instancetype)attributedExpression:(NSExpression *)expression fontNames:(nullable NSArray<NSString*> *)fontNames fontSize:(nullable NSNumber *)fontSize;

@end

NS_ASSUME_NONNULL_END
