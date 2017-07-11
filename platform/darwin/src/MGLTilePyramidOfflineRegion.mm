#import "MGLTilePyramidOfflineRegion.h"

#if !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
    #import <Cocoa/Cocoa.h>
#endif

#import "MGLOfflineRegion_Private.h"
#import "MGLGeometry_Private.h"
#import "MGLStyle.h"
#import "MGLTileID.h"
#include <mbgl/tile/tile_id.hpp>

@interface MGLTilePyramidOfflineRegion () <MGLOfflineRegion_Private>

@end

@implementation MGLTilePyramidOfflineRegion {
    NSURL *_styleURL;
}

@synthesize styleURL = _styleURL;

+ (BOOL)supportsSecureCoding {
    return YES;
}

- (instancetype)init {
    [NSException raise:@"Method unavailable"
                format:
     @"-[MGLTilePyramidOfflineRegion init] is unavailable. "
     @"Use -initWithStyleURL:bounds:fromZoomLevel:toZoomLevel: instead."];
    return nil;
}

- (instancetype)initWithStyleURL:(NSURL *)styleURL bounds:(MGLCoordinateBounds)bounds fromZoomLevel:(double)minimumZoomLevel toZoomLevel:(double)maximumZoomLevel tileList:(nullable NSArray*)tileList  {
    if (self = [self initWithStyleURL:styleURL bounds:bounds fromZoomLevel:minimumZoomLevel toZoomLevel:maximumZoomLevel]) {
        _tileList = tileList;
    }
    return self;
}

- (instancetype)initWithStyleURL:(NSURL *)styleURL bounds:(MGLCoordinateBounds)bounds fromZoomLevel:(double)minimumZoomLevel toZoomLevel:(double)maximumZoomLevel {
    if (self = [super init]) {
        if (!styleURL) {
            styleURL = [MGLStyle streetsStyleURLWithVersion:MGLStyleDefaultVersion];
        }

        if (!styleURL.scheme) {
            [NSException raise:@"Invalid style URL" format:
             @"%@ does not support setting a relative file URL as the style URL. "
             @"To download the online resources required by this style, "
             @"specify a URL to an online copy of this style. "
             @"For Mapbox-hosted styles, use the mapbox: scheme.",
             NSStringFromClass([self class])];
        }

        _styleURL = styleURL;
        _bounds = bounds;
        _minimumZoomLevel = minimumZoomLevel;
        _maximumZoomLevel = maximumZoomLevel;
        _tileList = nil;
    }
    return self;
}

- (instancetype)initWithOfflineRegionDefinition:(const mbgl::OfflineRegionDefinition &)definition {
    NSURL *styleURL = [NSURL URLWithString:@(definition.styleURL.c_str())];
    MGLCoordinateBounds bounds = MGLCoordinateBoundsFromLatLngBounds(definition.bounds);
    return [self initWithStyleURL:styleURL bounds:bounds fromZoomLevel:definition.minZoom toZoomLevel:definition.maxZoom tileList:nil];
}

- (const mbgl::OfflineRegionDefinition)offlineRegionDefinition {
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    const float scaleFactor = [UIScreen instancesRespondToSelector:@selector(nativeScale)] ? [[UIScreen mainScreen] nativeScale] : [[UIScreen mainScreen] scale];
#elif TARGET_OS_MAC
    const float scaleFactor = [NSScreen mainScreen].backingScaleFactor;
#endif
    
    std::vector<mbgl::CanonicalTileID> tileVector;
    tileVector.reserve(_tileList.count);
    for (NSNumber* value in _tileList) {
        MGLTileID tileId = MGLTileIDFromKey([value unsignedLongLongValue]);
        mbgl::CanonicalTileID canonicalTileID = mbgl::CanonicalTileID(tileId.z, tileId.x,tileId.y);
        tileVector.push_back(canonicalTileID);
    }
    return mbgl::OfflineTilePyramidRegionDefinition(_styleURL.absoluteString.UTF8String,
                                                    MGLLatLngBoundsFromCoordinateBounds(_bounds),
                                                    _minimumZoomLevel, _maximumZoomLevel,
                                                    scaleFactor, tileVector);
}

- (nullable instancetype)initWithCoder:(NSCoder *)coder {
    NSURL *styleURL = [coder decodeObjectForKey:@"styleURL"];
    CLLocationCoordinate2D sw = CLLocationCoordinate2DMake([coder decodeDoubleForKey:@"southWestLatitude"],
                                                           [coder decodeDoubleForKey:@"southWestLongitude"]);
    CLLocationCoordinate2D ne = CLLocationCoordinate2DMake([coder decodeDoubleForKey:@"northEastLatitude"],
                                                           [coder decodeDoubleForKey:@"northEastLongitude"]);
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(sw, ne);
    double minimumZoomLevel = [coder decodeDoubleForKey:@"minimumZoomLevel"];
    double maximumZoomLevel = [coder decodeDoubleForKey:@"maximumZoomLevel"];

    return [self initWithStyleURL:styleURL bounds:bounds fromZoomLevel:minimumZoomLevel toZoomLevel:maximumZoomLevel];
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_styleURL forKey:@"styleURL"];
    [coder encodeDouble:_bounds.sw.latitude forKey:@"southWestLatitude"];
    [coder encodeDouble:_bounds.sw.longitude forKey:@"southWestLongitude"];
    [coder encodeDouble:_bounds.ne.latitude forKey:@"northEastLatitude"];
    [coder encodeDouble:_bounds.ne.longitude forKey:@"northEastLongitude"];
    [coder encodeDouble:_maximumZoomLevel forKey:@"maximumZoomLevel"];
    [coder encodeDouble:_minimumZoomLevel forKey:@"minimumZoomLevel"];
}

- (id)copyWithZone:(nullable NSZone *)zone {
    return [[[self class] allocWithZone:zone] initWithStyleURL:_styleURL bounds:_bounds fromZoomLevel:_minimumZoomLevel toZoomLevel:_maximumZoomLevel];
}

- (BOOL)isEqual:(id)other {
    if (other == self) {
        return YES;
    }
    if (![other isKindOfClass:[self class]]) {
        return NO;
    }

    MGLTilePyramidOfflineRegion *otherRegion = other;
    return (_minimumZoomLevel == otherRegion->_minimumZoomLevel
            && _maximumZoomLevel == otherRegion->_maximumZoomLevel
            && MGLCoordinateBoundsEqualToCoordinateBounds(_bounds, otherRegion->_bounds)
            && [_styleURL isEqual:otherRegion->_styleURL]);
}

- (NSUInteger)hash {
    return (_styleURL.hash
            + @(_bounds.sw.latitude).hash + @(_bounds.sw.longitude).hash
            + @(_bounds.ne.latitude).hash + @(_bounds.ne.longitude).hash
            + @(_minimumZoomLevel).hash + @(_maximumZoomLevel).hash);
}

@end
