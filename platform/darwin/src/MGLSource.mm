#import "MGLSource.h"

#include <mbgl/style/source.hpp>

@implementation MGLSource

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier {
    if (self = [super init]) {
        _sourceIdentifier = sourceIdentifier;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbgl_source {
    [NSException raise:@"Subclasses must override this method" format:@""];
    return nil;
}

@end
