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
    if (_pendingSource == nullptr) {
        [NSException raise:@"MGLRedundantSourceException"
                    format:@"This instance %@ was already added to %@. Adding the same source instance " \
                            "to the style more than once is invalid.", self, mapView.style];
    }

    mapView.mbglMap->addSource(std::move(_pendingSource));
}

- (void)removeFromMapView:(MGLMapView *)mapView
{
    auto removedSource = mapView.mbglMap->removeSource(self.identifier.UTF8String);

    _pendingSource = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::VectorSource> &>(removedSource));
    self.rawSource = _pendingSource.get();
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; identifier = %@; URL = %@; tileSet = %@>",
            NSStringFromClass([self class]), (void *)self, self.identifier, self.URL, self.tileSet];
}

@end
