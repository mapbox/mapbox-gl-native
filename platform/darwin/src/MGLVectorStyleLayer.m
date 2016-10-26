#import "MGLVectorStyleLayer.h"

@implementation MGLVectorStyleLayer

- (void)setPredicate:(NSPredicate *)predicate {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLVectorLayer is an abstract class"];
}

- (NSPredicate *)predicate {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLVectorLayer is an abstract class"];
    return nil;
}

@end
