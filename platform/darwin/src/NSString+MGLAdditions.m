#import "NSString+MGLAdditions.h"

@implementation NSString (MGLAdditions)

- (NSRange)mgl_wholeRange
{
    return NSMakeRange(0, self.length);
}

- (nullable NSString *)mgl_stringOrNilIfEmpty
{
    return self.length ? self : nil;
}

@end

@implementation NSAttributedString (MGLAdditions)

- (NSRange)mgl_wholeRange
{
    return NSMakeRange(0, self.length);
}

@end
