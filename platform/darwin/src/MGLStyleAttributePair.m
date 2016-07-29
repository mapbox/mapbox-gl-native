#import "MGLStyleAttributePair.h"

@implementation MGLStyleAttributePair

+ (instancetype)pairWith:(id)t1 and:(id)t2
{
    MGLStyleAttributePair *pair = [[super alloc] init];
    if (pair == nil) return nil;
    pair.t1 = t1;
    pair.t2 = t2;
    return pair;
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<t1:%@ t2:%@>", _t1, _t2];
}

@end
