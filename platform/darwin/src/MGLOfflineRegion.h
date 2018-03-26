#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 An object conforming to the `MGLOfflineRegion` protocol determines which
 resources are required by an `MGLOfflinePack` object.
 */
@protocol MGLOfflineRegion <NSObject>

/**
 URL of the style whose resources are required for offline viewing.
 
 In addition to the JSON stylesheet, different styles may require different font
 glyphs, sprite sheets, and other resources.
 
 The URL may be a full HTTP or HTTPS URL or a Mapbox URL indicating the style’s
 map ID (`mapbox://styles/{user}/{style}`).
 */
@property (nonatomic, readonly) NSURL *styleURL;

@end

NS_ASSUME_NONNULL_END
