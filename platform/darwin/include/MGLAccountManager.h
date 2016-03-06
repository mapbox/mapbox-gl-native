#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/**
 The MGLAccountManager object provides a global way to set a Mapbox API access
 token, as well as other settings used framework-wide.
 */
@interface MGLAccountManager : NSObject

#pragma mark Authorizing Access

/**
 Set the [Mapbox access token](https://www.mapbox.com/help/define-access-token/)
 to be used by all instances of MGLMapView in the current application.
 
 Mapbox-hosted vector tiles and styles require an API access token, which you
 can obtain from the
 [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). Access
 tokens associate requests to Mapbox’s vector tile and style APIs with your
 Mapbox account. They also deter other developers from using your styles without
 your permission.
 
 @param accessToken A Mapbox access token. Calling this method with a value of
    `nil` has no effect.
 
 @note You must set the access token before attempting to load any Mapbox-hosted
    style. Therefore, you should generally set it before creating an instance of
    MGLMapView. The recommended way to set an access token is to add an entry to
    your application’s Info.plist file with the key `MGLMapboxAccessToken` and
    the type String. Alternatively, you may call this method from your
    application delegate’s `-applicationDidFinishLaunching:` method.
 */
+ (void)setAccessToken:(nullable NSString *)accessToken;

/**
 Returns the
 [Mapbox access token](https://www.mapbox.com/help/define-access-token/) in use
 by instances of MGLMapView in the current application.
 */
+ (nullable NSString *)accessToken;

+ (BOOL)mapboxMetricsEnabledSettingShownInApp __attribute__((deprecated("Telemetry settings are now always shown in the ℹ️ menu.")));

@end

NS_ASSUME_NONNULL_END
