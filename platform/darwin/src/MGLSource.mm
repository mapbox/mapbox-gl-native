#import "MGLSource_Private.h"

#include <mbgl/style/source.hpp>

@implementation MGLSource
{
    std::unique_ptr<mbgl::style::Source> _pendingSource;
}

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)pendingSource
{
    return std::move(_pendingSource);
}

- (void)setPendingSource:(std::unique_ptr<mbgl::style::Source>)pendingSource
{
    if (pendingSource != nullptr) {
        self.rawSource = pendingSource.get();
    }
    _pendingSource = std::move(pendingSource);
}

@end
