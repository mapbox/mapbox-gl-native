#import "MGLVectorStyleLayer.h"

@implementation MGLVectorStyleLayer

- (void)setPredicate:(NSPredicate *)predicate {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLVectorStyleLayer is an abstract class"];
}

- (NSPredicate *)predicate {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLVectorStyleLayer is an abstract class"];
    return nil;
}

- (NSString *)description {
    return [NSString stringWithFormat:
            @"<%@: %p; identifier = %@; sourceIdentifier = %@; "
            @"sourceLayerIdentifier = %@; predicate = %@; visible = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier,
            self.sourceIdentifier, self.sourceLayerIdentifier, self.predicate,
            self.visible ? @"YES" : @"NO"];
}

@end
