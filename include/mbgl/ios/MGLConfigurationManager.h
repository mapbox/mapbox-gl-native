#import "MGLTypes.h"
#import <CoreLocation/CoreLocation.h>

NS_ASSUME_NONNULL_BEGIN

/** The MGLConfigurationManager object provides a global way to set a Mapbox API access token, as well as other settings used framework-wide. */
@interface MGLConfigurationManager : NSObject

/** @name Authorizing Access */

/** Set the Mapbox API access token for the framework.
*
*   You can set an access token on MGLConfigurationManager or on an individual map view. The same token is used throughout the framework.
*   @param accessToken The Mapbox API access token. */
+ (void)setAccessToken:(nullable NSString *)accessToken;

/** Retreive the Mapbox API access token for the framework.
*
*   You can set an access token on MGLConfigurationManager or on an individual map view. The same token is used throughout the framework.
*   @return accessToken The Mapbox API access token. */
+ (nullable NSString *)accessToken;

/** @name Providing User Metrics Opt-Out */
+ (void)setMapboxMetricsEnabledSettingShownInApp:(BOOL)showsOptOut __attribute__((unavailable("Set MGLMapboxMetricsEnabledSettingShownInApp in Info.plist.")));

/** Whether in-app user metrics opt-out is configured. If set to the default value of `NO`, a user opt-out preference is expected in a `Settings.bundle` that shows in the application's section within the system Settings app. */
+ (BOOL)mapboxMetricsEnabledSettingShownInApp;

/** Set the type of activity associated with location updates for the application.
*
*   You can configure the activity type associated with location updates that make the most sense for youre application.
*   @param activityType The desired CLActivityType */
+ (void)setActivityType:(CLActivityType)activityType;

/** Retreive the type of activity associated with location updates for the application.
*
*   You can configure the activity type associated with location updates that make the most sense for your application.
*   @return activityType The configured CLActivityType*/
+ (CLActivityType)activityType;

@end

NS_ASSUME_NONNULL_END
