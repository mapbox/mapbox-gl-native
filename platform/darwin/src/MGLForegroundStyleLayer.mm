#import "MGLForegroundStyleLayer.h"
#import "MGLForegroundStyleLayer_Private.h"
#import "MGLStyleLayer_Private.h"
#import "MGLSource.h"

@implementation MGLForegroundStyleLayer

- (instancetype)initWithRawLayer:(mbgl::style::Layer *)rawLayer source:(MGLSource *)source {
    if (self = [super initWithRawLayer:rawLayer]) {
        _sourceIdentifier = source.identifier;
    }
    return self;
}

- (instancetype)initWithPendingLayer:(std::unique_ptr<mbgl::style::Layer>)pendingLayer source:(MGLSource *)source {
    if (self = [super initWithPendingLayer:std::move(pendingLayer)]) {
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
