#import "NSString+MGLAdditions.h"

@implementation NSString (MGLAdditions)

- (nullable NSString *)mgl_stringOrNilIfEmpty
{
    return self.length ? self : nil;
}

@end
