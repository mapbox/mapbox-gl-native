#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The MGLAccountManager object provides a global way to set a Mapbox API access token, as well as other settings used framework-wide. */
@interface MGLAccountManager : NSObject

/** @name Authorizing Access */

/** Set the Mapbox API access token for the framework.
*
*   You can set an access token on MGLAccountManager or on an individual map view. The same token is used throughout the framework.
*   @param accessToken The Mapbox API access token. */
+ (void)setAccessToken:(nullable NSString *)accessToken;

/** Retreive the Mapbox API access token for the framework.
*
*   You can set an access token on MGLAccountManager or on an individual map view. The same token is used throughout the framework.
*   @return accessToken The Mapbox API access token. */
+ (nullable NSString *)accessToken;

/** @name Providing User Metrics Opt-Out */

/** Certain Mapbox plans require the collection of user metrics. If you aren't using a preference switch in an existing or new `Settings.bundle` in your application, set this value to `YES` to indicate that you are providing a metrics opt-out for users within your app's interface directly.
*   @param showsOptOut Whether your application's interface provides a user opt-out preference. The default value is `NO`, meaning a `Settings.bundle` is expected for providing a user opt-out preference. */
+ (void)setMapboxMetricsEnabledSettingShownInApp:(BOOL)showsOptOut __attribute__((unavailable("Set MGLMapboxMetricsEnabledSettingShownInApp in Info.plist.")));

/** Whether in-app user metrics opt-out is configured. If set to the default value of `NO`, a user opt-out preference is expected in a `Settings.bundle` that shows in the application's section within the system Settings app. */
+ (BOOL)mapboxMetricsEnabledSettingShownInApp;

@end

NS_ASSUME_NONNULL_END
