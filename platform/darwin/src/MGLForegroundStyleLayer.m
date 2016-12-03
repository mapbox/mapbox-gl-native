#import "MGLForegroundStyleLayer.h"
#import "MGLSource.h"

@implementation MGLForegroundStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source {
    if (self = [super initWithIdentifier:identifier]) {
        _sourceIdentifier = source.identifier;
    }
    return self;
}

- (NSString *)description {
    return [NSString stringWithFormat:
            @"<%@: %p; identifier = %@; sourceIdentifier = %@; visible = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier,
            self.sourceIdentifier, self.visible ? @"YES" : @"NO"];
}

@end
