#import "MGLStyleLayer_Private.h"
#import "MGLSource.h"

@implementation MGLForegroundStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source {
    if (self = [super initWithIdentifier:identifier]) {
        _sourceIdentifier = source.identifier;
    }
    return self;
}

@end

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
