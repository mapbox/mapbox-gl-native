#import "NSString+MGLAdditions.h"

@implementation NSString (MGLAdditions)

- (NSString *)mgl_stringOrNilIfEmpty
{
    return self.length ? self : nil;
}

@end
