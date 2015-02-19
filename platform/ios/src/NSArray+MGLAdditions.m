#import "NSArray+MGLAdditions.h"

@implementation NSArray (MGLAdditions)

- (NSMutableArray *)deepMutableCopy
{
    return (NSMutableArray *)CFBridgingRelease(CFPropertyListCreateDeepCopy(kCFAllocatorDefault, (CFArrayRef)self, kCFPropertyListMutableContainersAndLeaves));
}

@end
