#import "MGLVectorSource.h"

#import "MGLSource_Private.h"

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

- (instancetype)initWithSourceIdentifier:(NSString *)sourceIdentifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates minimumZoomLevel:(NSUInteger)minimumZoomLevel maximumZoomLevel:(NSUInteger)maximumZoomLevel
{
    if (self = [super initWithSourceIdentifier:sourceIdentifier])
    {
        _tileURLTemplates = tileURLTemplates;
        _minimumZoomLevel = minimumZoomLevel;
        _maximumZoomLevel = maximumZoomLevel;
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
        mbgl::Tileset tileSource;
        tileSource.zoomRange = mbgl::Range<uint8_t>(self.minimumZoomLevel, self.maximumZoomLevel);
        for (NSString *tileURLTemplate in self.tileURLTemplates)
        {
            tileSource.tiles.push_back(tileURLTemplate.UTF8String);
        }
        source = std::make_unique<mbgl::style::VectorSource>(self.sourceIdentifier.UTF8String, tileSource);
    }
    
    return std::move(source);
}

@end
