#import "MGLRasterSource_Private.h"

#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSource_Private.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/raster_source.hpp>

const MGLTileSourceOption MGLTileSourceOptionTileSize = @"MGLTileSourceOptionTileSize";

static const CGFloat MGLRasterSourceClassicTileSize = 256;
static const CGFloat MGLRasterSourceRetinaTileSize = 512;

@interface MGLRasterSource ()

- (instancetype)initWithRawSource:(mbgl::style::RasterSource *)rawSource NS_DESIGNATED_INITIALIZER;

@property (nonatomic) mbgl::style::RasterSource *rawSource;

@end

@implementation MGLRasterSource {
    std::unique_ptr<mbgl::style::RasterSource> _pendingSource;
}

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL {
    // The style specification default is 512, but 256 is the expected value for
    // any tile set that would be accessed through a mapbox: URL and therefore
    // any tile URL that this option currently affects.
    BOOL isMapboxURL = ([configurationURL.scheme isEqualToString:@"mapbox"]
                        && [configurationURL.host containsString:@"."]
                        && (!configurationURL.path.length || [configurationURL.path isEqualToString:@"/"]));
    CGFloat tileSize = isMapboxURL ? MGLRasterSourceClassicTileSize : MGLRasterSourceRetinaTileSize;
    return [self initWithIdentifier:identifier configurationURL:configurationURL tileSize:tileSize];
}

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL tileSize:(CGFloat)tileSize {
    if (self = [super initWithIdentifier:identifier configurationURL:configurationURL]) {
        auto source = std::make_unique<mbgl::style::RasterSource>(identifier.UTF8String,
                                                                  configurationURL.mgl_URLByStandardizingScheme.absoluteString.UTF8String,
                                                                  uint16_t(round(tileSize)));
        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();
    }
    return self;
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options {
    if (self = [super initWithIdentifier:identifier tileURLTemplates:tileURLTemplates options:options]) {
        mbgl::Tileset tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, options);
        
        uint16_t tileSize = MGLRasterSourceRetinaTileSize;
        if (NSNumber *tileSizeNumber = options[MGLTileSourceOptionTileSize]) {
            if (![tileSizeNumber isKindOfClass:[NSNumber class]]) {
                [NSException raise:NSInvalidArgumentException
                            format:@"MGLTileSourceOptionTileSize must be set to an NSNumber."];
            }
            tileSize = static_cast<uint16_t>(round(tileSizeNumber.doubleValue));
        }
        
        auto source = std::make_unique<mbgl::style::RasterSource>(identifier.UTF8String, tileSet, tileSize);
        _pendingSource = std::move(source);
        self.rawSource = _pendingSource.get();
    }
    return self;
}

- (instancetype)initWithRawSource:(mbgl::style::RasterSource *)rawSource {
    return [super initWithRawSource:rawSource];
}

- (void)addToMapView:(MGLMapView *)mapView {
    if (_pendingSource == nullptr) {
        [NSException raise:@"MGLRedundantSourceException"
                    format:@"This instance %@ was already added to %@. Adding the same source instance " \
                           @"to the style more than once is invalid.", self, mapView.style];
    }

    mapView.mbglMap->addSource(std::move(_pendingSource));
}

- (void)removeFromMapView:(MGLMapView *)mapView {
    auto removedSource = mapView.mbglMap->removeSource(self.identifier.UTF8String);

    _pendingSource = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::RasterSource> &>(removedSource));
    self.rawSource = _pendingSource.get();
}

- (mbgl::style::RasterSource *)rawSource {
    return (mbgl::style::RasterSource *)super.rawSource;
}

- (void)setRawSource:(mbgl::style::RasterSource *)rawSource {
    super.rawSource = rawSource;
}

- (NSURL *)configurationURL {
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (NSString *)attributionHTMLString {
    auto attribution = self.rawSource->getAttribution();
    return attribution ? @(attribution->c_str()) : nil;
}

@end
