#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

typedef NSString * MGLAttributedExpressionKey NS_EXTENSIBLE_STRING_ENUM;

FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontNamesAttribute;
FOUNDATION_EXTERN MGL_EXPORT MGLAttributedExpressionKey const MGLFontSizeAttribute;

@interface MGLAttributedExpression : NSObject

@property (strong, nonatomic) NSExpression *expression;
@property (strong, nonatomic, readonly) NSDictionary<MGLAttributedExpressionKey, id> *attributes;

- (instancetype)initWithExpression:(NSExpression *)expression;
- (instancetype)initWithExpression:(NSExpression *)expression attributes:(nullable NSDictionary <MGLAttributedExpressionKey, id> *)attrs;

+ (instancetype)initWithExpression:(NSExpression *)expression fontNames:(nullable NSArray<NSString*> *)fontNames fontSize:(nullable NSNumber *)fontSize;

@end

NS_ASSUME_NONNULL_END
