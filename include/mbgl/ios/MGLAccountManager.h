@interface MGLAccountManager : NSObject

+ (void) setMapboxMetricsEnabledSettingShownInApp:(BOOL)showsOptOut;
+ (void) setAccessToken:(NSString *) accessToken;
+ (NSString *) accessToken;

@end