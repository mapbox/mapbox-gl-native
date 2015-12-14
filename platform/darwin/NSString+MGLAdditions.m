#import "NSString+MGLAdditions.h"

void mgl_linkStringCategory() {}

@implementation NSString (MGLAdditions)

- (nullable NSString *)mgl_stringOrNilIfEmpty
{
    return self.length ? self : nil;
}

@end
