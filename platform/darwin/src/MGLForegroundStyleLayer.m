#import "MGLForegroundStyleLayer.h"
#import "MGLSource.h"

@implementation MGLForegroundStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source {
    if (self = [super initWithIdentifier:identifier]) {
        _sourceIdentifier = source.identifier;
    }
    return self;
}

@end
