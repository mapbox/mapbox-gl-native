#import "MGLShapeOfflineRegion.h"

#if !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
    #import <Cocoa/Cocoa.h>
#else
    #import <UIKit/UIKit.h>
#endif

#import "MGLOfflineRegion_Private.h"
#import "MGLShapeOfflineRegion_Private.h"
#import "MGLFeature_Private.h"
#import "MGLShape_Private.h"
#import "MGLStyle.h"
#import "MGLLoggingConfiguration_Private.h"

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
#import "MMEConstants.h"
#endif

@interface MGLShapeOfflineRegion () <MGLOfflineRegion_Private, MGLShapeOfflineRegion_Private>

@end

@implementation MGLShapeOfflineRegion {
    NSURL *_styleURL;
}

@synthesize styleURL = _styleURL;
@synthesize includesIdeographicGlyphs = _includesIdeographicGlyphs;

-(NSDictionary *)offlineStartEventAttributes {
    return @{
             #if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
             MMEEventKeyShapeForOfflineRegion: @"shaperegion",
             MMEEventKeyMinZoomLevel: @(self.minimumZoomLevel),
             MMEEventKeyMaxZoomLevel: @(self.maximumZoomLevel),
             MMEEventKeyStyleURL: self.styleURL.absoluteString ?: [NSNull null]
             #endif
             };
}

+ (BOOL)supportsSecureCoding {
    return YES;
}

- (instancetype)init {
    MGLLogInfo(@"Calling this initializer is not allowed.");
    [NSException raise:@"Method unavailable"
                format:
     @"-[MGLShapeOfflineRegion init] is unavailable. "
     @"Use -initWithStyleURL:shape:fromZoomLevel:toZoomLevel: instead."];
    return nil;
}

- (instancetype)initWithStyleURL:(NSURL *)styleURL shape:(MGLShape *)shape fromZoomLevel:(double)minimumZoomLevel toZoomLevel:(double)maximumZoomLevel {
    MGLLogDebug(@"Initializing styleURL: %@ shape: %@ fromZoomLevel: %f toZoomLevel: %f", styleURL, shape, minimumZoomLevel, maximumZoomLevel);
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
        _shape = shape;
        _minimumZoomLevel = minimumZoomLevel;
        _maximumZoomLevel = maximumZoomLevel;
        _includesIdeographicGlyphs = NO;
    }
    return self;
}

- (instancetype)initWithOfflineRegionDefinition:(const mbgl::OfflineGeometryRegionDefinition &)definition {
    NSURL *styleURL = [NSURL URLWithString:@(definition.styleURL.c_str())];
    MGLShape *shape = MGLShapeFromGeoJSON(definition.geometry);
    MGLShapeOfflineRegion* result = [self initWithStyleURL:styleURL shape:shape fromZoomLevel:definition.minZoom toZoomLevel:definition.maxZoom];
    result.includesIdeographicGlyphs = definition.includeIdeographs;
    return result;
}

- (const mbgl::OfflineRegionDefinition)offlineRegionDefinition {
#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
    const float scaleFactor = [UIScreen instancesRespondToSelector:@selector(nativeScale)] ? [[UIScreen mainScreen] nativeScale] : [[UIScreen mainScreen] scale];
#elif TARGET_OS_MAC
    const float scaleFactor = [NSScreen mainScreen].backingScaleFactor;
#endif
    return mbgl::OfflineGeometryRegionDefinition(_styleURL.absoluteString.UTF8String,
                                                 _shape.geometryObject,
                                                 _minimumZoomLevel, _maximumZoomLevel,
                                                 scaleFactor, _includesIdeographicGlyphs);
}

- (nullable instancetype)initWithCoder:(NSCoder *)coder {
    MGLLogInfo(@"Initializing with coder.");
    NSURL *styleURL = [coder decodeObjectForKey:@"styleURL"];
    MGLShape * shape = [coder decodeObjectForKey:@"shape"];
    double minimumZoomLevel = [coder decodeDoubleForKey:@"minimumZoomLevel"];
    double maximumZoomLevel = [coder decodeDoubleForKey:@"maximumZoomLevel"];

    MGLShapeOfflineRegion* result = [self initWithStyleURL:styleURL shape:shape fromZoomLevel:minimumZoomLevel toZoomLevel:maximumZoomLevel];
    result.includesIdeographicGlyphs = [coder decodeBoolForKey:@"includesIdeographicGlyphs"];
    return result;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeObject:_styleURL forKey:@"styleURL"];
    [coder encodeObject:_shape forKey:@"shape"];
    [coder encodeDouble:_maximumZoomLevel forKey:@"maximumZoomLevel"];
    [coder encodeDouble:_minimumZoomLevel forKey:@"minimumZoomLevel"];
    [coder encodeBool:_includesIdeographicGlyphs forKey:@"includesIdeographicGlyphs"];
}

- (id)copyWithZone:(nullable NSZone *)zone {
    MGLShapeOfflineRegion* result = [[[self class] allocWithZone:zone] initWithStyleURL:_styleURL shape:_shape fromZoomLevel:_minimumZoomLevel toZoomLevel:_maximumZoomLevel];
    result.includesIdeographicGlyphs = _includesIdeographicGlyphs;
    return result;
}

- (BOOL)isEqual:(id)other {
    if (other == self) {
        return YES;
    }
    if (![other isKindOfClass:[self class]]) {
        return NO;
    }

    MGLShapeOfflineRegion *otherRegion = other;
    return (_minimumZoomLevel == otherRegion->_minimumZoomLevel
            && _maximumZoomLevel == otherRegion->_maximumZoomLevel
            && _shape.geometryObject == otherRegion->_shape.geometryObject
            && [_styleURL isEqual:otherRegion->_styleURL]
            && _includesIdeographicGlyphs == otherRegion->_includesIdeographicGlyphs);
}

- (NSUInteger)hash {
    return (_styleURL.hash
            + _shape.hash
            + @(_minimumZoomLevel).hash + @(_maximumZoomLevel).hash
            + @(_includesIdeographicGlyphs).hash);
}

@end
