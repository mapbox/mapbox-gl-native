#import "MGLAttributedExpression.h"
#import "MGLLoggingConfiguration_Private.h"

const MGLAttributedExpressionKey MGLFontNamesAttribute = @"text-font";
const MGLAttributedExpressionKey MGLFontScaleAttribute = @"font-scale";
const MGLAttributedExpressionKey MGLFontColorAttribute = @"text-color";

@implementation MGLAttributedExpression

- (instancetype)initWithExpression:(NSExpression *)expression {
    self = [self initWithExpression:expression attributes:@{}];
    return self;
}

+ (instancetype)attributedExpression:(NSExpression *)expression fontNames:(nullable NSArray<NSString *> *)fontNames fontScale:(nullable NSNumber *)fontScale {
    MGLAttributedExpression *attributedExpression;
    
    NSMutableDictionary *attrs = [NSMutableDictionary dictionary];
    
    if (fontNames && fontNames.count > 0) {
        attrs[MGLFontNamesAttribute] = [NSExpression expressionForConstantValue:fontNames];
    }
    
    if (fontScale) {
        attrs[MGLFontScaleAttribute] = [NSExpression expressionForConstantValue:fontScale];
    }
    
    attributedExpression = [[self alloc] initWithExpression:expression attributes:attrs];
    return attributedExpression;
}

+ (instancetype)attributedExpression:(NSExpression *)expression attributes:(nonnull NSDictionary<MGLAttributedExpressionKey, NSExpression *> *)attrs {
    MGLAttributedExpression *attributedExpression;
    
    attributedExpression = [[self alloc] initWithExpression:expression attributes:attrs];
    
    return attributedExpression;
}

- (instancetype)initWithExpression:(NSExpression *)expression attributes:(nonnull NSDictionary<MGLAttributedExpressionKey, NSExpression *> *)attrs {
    if (self = [super init])
    {
        MGLLogInfo(@"Starting %@ initialization.", NSStringFromClass([self class]));
        _expression = expression;
        _attributes = attrs;
        
        MGLLogInfo(@"Finalizing %@ initialization.", NSStringFromClass([self class]));
    }
    return self;
}

- (BOOL)isEqual:(id)object {
    BOOL result = NO;
    
    if ([object isKindOfClass:[self class]]) {
        MGLAttributedExpression *otherObject = object;
        result = [self.expression isEqual:otherObject.expression] &&
        [_attributes isEqual:otherObject.attributes];
    }
    
    return result;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"MGLAttributedExpression<Expression: %@ Attributes: %@>", self.expression, self.attributes];
}

@end
