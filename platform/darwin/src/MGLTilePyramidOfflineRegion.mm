#import "MGLTilePyramidOfflineRegion.h"

#if !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
    #import <Cocoa/Cocoa.h>
#endif

#import "MGLOfflineRegion_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLStyle.h"

@interface MGLTilePyramidOfflineRegion () <MGLOfflineRegion_Private>

@end

@implementation MGLTilePyramidOfflineRegion {
    NSURL *_styleURL;
}

@synthesize styleURL = _styleURL;

- (instancetype)init {
    [NSException raise:@"Method unavailable"
                format:
     @"-[MGLTilePyramidOfflineRegion init] is unavailable. "
     @"Use -initWithStyleURL:bounds:fromZoomLevel:toZoomLevel: instead."];
    return nil;
}

- (instancetype)initWithStyleURL:(NSURL *)styleURL bounds:(MGLCoordinateBounds)bounds fromZoomLevel:(double)minimumZoomLevel toZoomLevel:(double)maximumZoomLevel {
    if (self = [super init]) {
        if (!styleURL) {
            styleURL = [MGLStyle streetsStyleURL];
        }
        
        if (!styleURL.scheme) {
            // Assume a relative path into the application bundle.
            styleURL = [NSURL URLWithString:[@"asset://" stringByAppendingString:styleURL.absoluteString]];
        }
        
        _styleURL = styleURL;
        _bounds = bounds;
        _minimumZoomLevel = minimumZoomLevel;
        _maximumZoomLevel = maximumZoomLevel;
    }
    return self;
}

- (instancetype)initWithOfflineRegionDefinition:(const mbgl::OfflineRegionDefinition &)definition {
    NSURL *styleURL = [NSURL URLWithString:@(definition.styleURL.c_str())];
    MGLCoordinateBounds bounds = MGLCoordinateBoundsFromLatLngBounds(definition.bounds);
    return [self initWithStyleURL:styleURL bounds:bounds fromZoomLevel:definition.minZoom toZoomLevel:definition.maxZoom];
}

- (const mbgl::OfflineRegionDefinition)offlineRegionDefinition {
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    const float scaleFactor = [UIScreen instancesRespondToSelector:@selector(nativeScale)] ? [[UIScreen mainScreen] nativeScale] : [[UIScreen mainScreen] scale];
#elif TARGET_OS_MAC
    const float scaleFactor = [NSScreen mainScreen].backingScaleFactor;
#endif
    return mbgl::OfflineTilePyramidRegionDefinition(_styleURL.absoluteString.UTF8String,
                                                    MGLLatLngBoundsFromCoordinateBounds(_bounds),
                                                    _minimumZoomLevel, _maximumZoomLevel,
                                                    scaleFactor);
}

@end
