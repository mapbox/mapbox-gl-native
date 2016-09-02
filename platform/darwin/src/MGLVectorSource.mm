#import "MGLVectorSource.h"

#import "MGLSource_Private.h"

#include <mbgl/style/sources/vector_source.hpp>

@implementation MGLVectorSource

static NSString *MGLVectorSourceType   = @"vector";

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url {
    if (self = [super initWithSourceIdentifier:sourceIdentifier]) {
        _URL = url;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbgl_source
{
    auto source = std::make_unique<mbgl::style::VectorSource>(self.sourceIdentifier.UTF8String, self.URL.absoluteString.UTF8String);
    return std::move(source);
}

@end
