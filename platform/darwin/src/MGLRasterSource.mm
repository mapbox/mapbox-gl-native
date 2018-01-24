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

@property (nonatomic, readonly) mbgl::style::RasterSource *rawSource;

@end

@implementation MGLRasterSource

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
    auto source = [self pendingSourceWithIdentifier:identifier configurationURL:configurationURL tileSize:tileSize];
    return self = [super initWithPendingSource:std::move(source)];
}

- (std::unique_ptr<mbgl::style::RasterSource>)pendingSourceWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL tileSize:(CGFloat)tileSize {
    NSString *configurationURLString = configurationURL.mgl_URLByStandardizingScheme.absoluteString;
    return std::make_unique<mbgl::style::RasterSource>(identifier.UTF8String,
                                                       configurationURLString.UTF8String,
                                                       uint16_t(round(tileSize)));
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NS_ARRAY_OF(NSString *) *)tileURLTemplates options:(nullable NS_DICTIONARY_OF(MGLTileSourceOption, id) *)options {
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
    return self = [super initWithPendingSource:std::move(source)];
}

- (mbgl::style::RasterSource *)rawSource {
    return (mbgl::style::RasterSource *)super.rawSource;
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
