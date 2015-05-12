@interface MGLAccountManager : NSObject

+ (void) setMapboxMetricsEnabledSettingShownInApp:(BOOL)showsOptOut;
+ (BOOL) mapboxMetricsEnabledSettingShownInApp;
+ (void) setAccessToken:(NSString *) accessToken;
+ (NSString *) accessToken;

@end