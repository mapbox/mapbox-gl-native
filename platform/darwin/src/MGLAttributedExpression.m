#import "MGLAttributedExpression.h"
#import "MGLLoggingConfiguration_Private.h"

const MGLAttributedExpressionKey MGLFontNamesAttribute = @"";
const MGLAttributedExpressionKey MGLFontSizeAttribute = @"";

@implementation MGLAttributedExpression

- (instancetype)initWithExpression:(NSExpression *)expression {
    self = [self initWithExpression:expression attributes:nil];
    return self;
}

+ (instancetype)initWithExpression:(NSExpression *)expression fontNames:(nullable NSArray<NSString *> *)fontNames fontSize:(nullable NSNumber *)fontSize {
    MGLAttributedExpression *attributedExpression;
    
    NSMutableDictionary *attrs = [NSMutableDictionary dictionary];
    
    if (fontNames && fontNames.count > 0) {
        [attrs setObject:fontNames forKey:MGLFontNamesAttribute];
    }
    
    if (fontSize) {
        [attrs setObject:fontSize forKey:MGLFontSizeAttribute];
    }
    
    attributedExpression = [[self alloc] initWithExpression:expression attributes:attrs];
    return attributedExpression;
}

- (instancetype)initWithExpression:(NSExpression *)expression attributes:(NSDictionary<MGLAttributedExpressionKey,id> *)attrs {
    if (self = [super init])
    {
        MGLLogInfo(@"Starting %@ initialization.", NSStringFromClass([self class]));
        _expression = expression;
        _attributes = attrs;
        
        MGLLogInfo(@"Finalizing %@ initialization.", NSStringFromClass([self class]));
    }
    return self;
}

@end
