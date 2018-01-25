#import "MGLRasterDEMSource.h"

#import "MGLRasterSource_Private.h"
#import "NSURL+MGLAdditions.h"

#import <mbgl/style/sources/raster_dem_source.hpp>

@implementation MGLRasterDEMSource

- (std::unique_ptr<mbgl::style::RasterSource>)pendingSourceWithIdentifier:(NSString *)identifier configurationURL:(NSURL *)configurationURL tileSize:(CGFloat)tileSize {
    NSString *configurationURLString = configurationURL.mgl_URLByStandardizingScheme.absoluteString;
    return std::make_unique<mbgl::style::RasterDEMSource>(identifier.UTF8String,
                                                          configurationURLString.UTF8String,
                                                          uint16_t(round(tileSize)));
}

@end
