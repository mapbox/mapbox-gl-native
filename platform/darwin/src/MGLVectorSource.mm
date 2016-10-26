#import "MGLVectorSource.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSet_Private.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/vector_source.hpp>

@interface MGLVectorSource ()

@property (nonatomic) mbgl::style::VectorSource *rawSource;

@end

@implementation MGLVectorSource
{
    std::unique_ptr<mbgl::style::VectorSource> _pendingSource;
}

static NSString *MGLVectorSourceType   = @"vector";

- (instancetype)initWithIdentifier:(NSString *)identifier URL:(NSURL *)url
{
    if (self = [super initWithIdentifier:identifier])
    {
        _URL = url;
        [self commonInit];
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileSet:(MGLTileSet *)tileSet
{
    if (self = [super initWithIdentifier:identifier])
    {
        _tileSet = tileSet;
        [self commonInit];
    }
    return self;
}

- (void)commonInit
{
    std::unique_ptr<mbgl::style::VectorSource> source;
    
    if (self.URL)
    {
        source = std::make_unique<mbgl::style::VectorSource>(self.identifier.UTF8String,
                                                             self.URL.mgl_URLByStandardizingScheme.absoluteString.UTF8String);
    }
    else
    {
        source = std::make_unique<mbgl::style::VectorSource>(self.identifier.UTF8String,
                                                             self.tileSet.mbglTileset);
    }
    
    _pendingSource = std::move(source);
    self.rawSource = _pendingSource.get();
}

- (void)addToMapView:(MGLMapView *)mapView
{
    mapView.mbglMap->addSource(std::move(_pendingSource));
}

@end
