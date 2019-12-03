#import "MGLRasterTileSource_Private.h"

#import "MGLLoggingConfiguration_Private.h"
#import "MGLMapView_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSource_Private.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/sources/raster_source.hpp>

const MGLTileSourceOption MGLTileSourceOptionTileSize = @"MGLTileSourceOptionTileSize";

static const CGFloat MGLRasterTileSourceClassicTileSize = 256;
static const CGFloat MGLRasterTileSourceRetinaTileSize = 512;

@interface MGLRasterTileSource ()

@property (nonatomic, readonly) mbgl::style::RasterSource *rawSource;

@end

@implementation MGLRasterTileSource

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL {
    // The style specification default is 512, but 256 is the expected value for
    // any tile set that would be accessed through a mapbox: URL and therefore
    // any tile URL that this option currently affects.
    BOOL isMapboxURL = ([configurationURL.scheme isEqualToString:@"mapbox"]
                        && [configurationURL.host containsString:@"."]
                        && (!configurationURL.path.length || [configurationURL.path isEqualToString:@"/"]));
    CGFloat tileSize = isMapboxURL ? MGLRasterTileSourceClassicTileSize : MGLRasterTileSourceRetinaTileSize;
    return [self initWithIdentifier:identifier configurationURL:configurationURL tileSize:tileSize];
}

- (instancetype)initWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL tileSize:(CGFloat)tileSize {
    NSString *configurationURLString = configurationURL.mgl_URLByStandardizingScheme.absoluteString;
    auto source = [self pendingSourceWithIdentifier:identifier urlOrTileset:configurationURLString.UTF8String tileSize:uint16_t(round(tileSize))];
    return self = [super initWithPendingSource:std::move(source)];
}

- (std::unique_ptr<mbgl::style::RasterSource>)pendingSourceWithIdentifier:(NSString *)identifier urlOrTileset:(mbgl::variant<std::string, mbgl::Tileset>)urlOrTileset tileSize:(uint16_t)tileSize {
    auto source = std::make_unique<mbgl::style::RasterSource>(identifier.UTF8String,
                                                              urlOrTileset,
                                                              tileSize);
    return source;
}

- (instancetype)initWithIdentifier:(NSString *)identifier tileURLTemplates:(NSArray<NSString *> *)tileURLTemplates options:(nullable NSDictionary<MGLTileSourceOption, id> *)options {
    mbgl::Tileset tileSet = MGLTileSetFromTileURLTemplates(tileURLTemplates, options);

    uint16_t tileSize = MGLRasterTileSourceRetinaTileSize;
    if (NSNumber *tileSizeNumber = options[MGLTileSourceOptionTileSize]) {
        if (![tileSizeNumber isKindOfClass:[NSNumber class]]) {
            [NSException raise:NSInvalidArgumentException
                        format:@"MGLTileSourceOptionTileSize must be set to an NSNumber."];
        }
        tileSize = static_cast<uint16_t>(round(tileSizeNumber.doubleValue));
    }

    auto source = [self pendingSourceWithIdentifier:identifier urlOrTileset:tileSet tileSize:tileSize];
    return self = [super initWithPendingSource:std::move(source)];
}

- (mbgl::style::RasterSource *)rawSource {
    return (mbgl::style::RasterSource *)super.rawSource;
}

- (NSURL *)configurationURL {
    MGLAssertStyleSourceIsValid();
    auto url = self.rawSource->getURL();
    return url ? [NSURL URLWithString:@(url->c_str())] : nil;
}

- (NSString *)attributionHTMLString {
    if (!self.rawSource) {
        MGLAssert(0, @"Source with identifier `%@` was invalidated after a style change", self.identifier);
        return nil;
    }

    auto attribution = self.rawSource->getAttribution();
    return attribution ? @(attribution->c_str()) : nil;
}

@end
