#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The MGLAccountManager object provides a global way to set a Mapbox API access token, as well as other settings used framework-wide. */
__deprecated
@interface MGLAccountManager : NSObject

/** @name Authorizing Access */

/** Set the Mapbox API access token for the framework.
*
*   You can set an access token on MGLConfigurationManager or on an individual map view. The same token is used throughout the framework.
*   @param accessToken The Mapbox API access token. */
+ (void)setAccessToken:(nullable NSString *)accessToken __deprecated_msg("Use + setAccessToken in MGLConfigurationManager");

/** Retreive the Mapbox API access token for the framework.
*
*   You can set an access token on MGLConfigurationManager or on an individual map view. The same token is used throughout the framework.
*   @return accessToken The Mapbox API access token. */
+ (nullable NSString *)accessToken __deprecated_msg("Use + accessToken in MGLConfigurationManager");

/** @name Providing User Metrics Opt-Out */
+ (void)setMapboxMetricsEnabledSettingShownInApp:(BOOL)showsOptOut __attribute__((unavailable("Set MGLMapboxMetricsEnabledSettingShownInApp in Info.plist.")));

/** Whether in-app user metrics opt-out is configured. If set to the default value of `NO`, a user opt-out preference is expected in a `Settings.bundle` that shows in the application's section within the system Settings app. */
+ (BOOL)mapboxMetricsEnabledSettingShownInApp __deprecated_msg("Use + mapboxMetricsEnabledSettingShownInApp in MGLConfigurationManager");

@end

NS_ASSUME_NONNULL_END