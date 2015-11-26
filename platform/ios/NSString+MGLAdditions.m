#import "NSString+MGLAdditions.h"

@implementation NSString (MGLAdditions)

void mgl_linkStringCategory(){}

- (nullable NSString *)mgl_stringOrNilIfEmpty
{
    return self.length ? self : nil;
}

@end
