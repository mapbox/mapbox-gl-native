#import "MGLAccountManager.h"
#import "MGLConfigurationManager.h"

@implementation MGLAccountManager

+ (void)setAccessToken:(nullable NSString *)accessToken {
    [MGLConfigurationManager setAccessToken:accessToken];
}

+ (nullable NSString *)accessToken {
    return [MGLConfigurationManager accessToken];
}

+ (BOOL)mapboxMetricsEnabledSettingShownInApp {
    return [MGLConfigurationManager mapboxMetricsEnabledSettingShownInApp];
}

@end