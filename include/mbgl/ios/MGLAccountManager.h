#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** The MGLAccountManager object provides a global way to set a Mapbox API access token, as well as other settings used framework-wide. */
@interface MGLAccountManager : NSObject

#pragma mark Authorizing Access

/**
 Set the Mapbox API access token for the framework.
 
 You can set an access token on MGLAccountManager or on an individual map view. The same token is used throughout the framework.
    @param accessToken The Mapbox API access token.
 */
+ (void)setAccessToken:(nullable NSString *)accessToken;

/**
 Retreive the Mapbox API access token for the framework.
 
 You can set an access token on MGLAccountManager or on an individual map view. The same token is used throughout the framework.
 
 @return accessToken The Mapbox API access token.
 */
+ (nullable NSString *)accessToken;

+ (BOOL)mapboxMetricsEnabledSettingShownInApp __attribute__((deprecated("Telemetry settings are now always shown in the ℹ️ menu.")));

@end

NS_ASSUME_NONNULL_END
