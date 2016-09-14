#import "MGLVectorSource.h"

#import "MGLSource_Private.h"
#import "MGLTileSet_Private.h"

#include <mbgl/style/sources/vector_source.hpp>

@implementation MGLVectorSource

static NSString *MGLVectorSourceType   = @"vector";

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier URL:(NSURL *)url
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _URL = url;
    }
    return self;
}

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileSet:(MGLTileSet *)tileSet
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _tileSet = tileSet;
    }
    return self;
}

- (std::unique_ptr<mbgl::style::Source>)mbglSource
{
    std::unique_ptr<mbgl::style::VectorSource> source;
    
    if (self.URL)
    {
        source = std::make_unique<mbgl::style::VectorSource>(self.sourceIdentifier.UTF8String, self.URL.absoluteString.UTF8String);
    }
    else
    {
        source = std::make_unique<mbgl::style::VectorSource>(self.sourceIdentifier.UTF8String, self.tileSet.mbglTileset);
    }
    
    return std::move(source);
}

@end
