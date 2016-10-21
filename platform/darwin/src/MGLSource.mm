#import "MGLSource_Private.h"

#include <mbgl/style/source.hpp>

@interface MGLSource ()

@end

@implementation MGLSource
{
    mbgl::style::Source *_rawSource;
    std::unique_ptr<mbgl::style::Source> _source;
}

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super init]) {
        _identifier = identifier;
    }
    return self;
}

- (mbgl::style::Source *)getRawSource
{
    return _rawSource;
}

- (void)setRawSource:(mbgl::style::Source *)rawSource
{
    _rawSource = rawSource;
}

- (std::unique_ptr<mbgl::style::Source>)source
{
    return std::move(_source);
}

- (void)setSource:(std::unique_ptr<mbgl::style::Source>)source
{
    if (source != nullptr) {
        _rawSource = source.get();
    }
    _source = std::move(source);
}

@end
