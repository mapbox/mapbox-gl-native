#import "MGLForegroundStyleLayer.h"

@implementation MGLForegroundStyleLayer

- (NSString *)sourceIdentifier {
    [NSException raise:@"MGLAbstractClassException"
                format:@"MGLForegroundStyleLayer is an abstract class"];
    return nil;
}

- (NSString *)description {
    return [NSString stringWithFormat:
            @"<%@: %p; identifier = %@; sourceIdentifier = %@; visible = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier,
            self.sourceIdentifier, self.visible ? @"YES" : @"NO"];
}

@end
