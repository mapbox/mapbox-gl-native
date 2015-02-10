#import "NSDictionary+MGLAdditions.h"

@implementation NSDictionary (MGLAdditions)

- (NSMutableDictionary *)deepMutableCopy
{
    return (NSMutableDictionary *)CFBridgingRelease(CFPropertyListCreateDeepCopy(kCFAllocatorDefault, (CFDictionaryRef)self, kCFPropertyListMutableContainersAndLeaves));
}

@end
