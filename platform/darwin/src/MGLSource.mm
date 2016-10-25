#import "MGLSource_Private.h"

#include <mbgl/style/source.hpp>

@implementation MGLSource
{
    std::unique_ptr<mbgl::style::Source> _source;
}

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)source
{
    return std::move(_source);
}

- (void)setSource:(std::unique_ptr<mbgl::style::Source>)source
{
    if (source != nullptr) {
        self.rawSource = source.get();
    }
    _source = std::move(source);
}

@end
