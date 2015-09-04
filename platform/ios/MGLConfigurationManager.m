#import "MGLConfigurationManager_Private.h"
#import "MGLMapboxEvents.h"
#import "NSProcessInfo+MGLAdditions.h"

static NSString * const kMGLMapboxMetricsEnabledSettingShownInApp = @"MGLMapboxMetricsEnabledSettingShownInApp";
static NSString * const kMGLMapboxAccessToken = @"MGLMapboxAccessToken";
static NSString * const kMGLMapboxActivityType = @"MGLMapboxActivityType";

// Used for parsing options specified in info.plist
static NSString * const kMGLMapboxActivityTypeOther = @"Other";
static NSString * const kMGLMapboxActivityTypeAutomotiveNavigation = @"AutomotiveNavigation";
static NSString * const kMGLMapboxActivityTypeFitness = @"Fitness";
static NSString * const kMGLMapboxActivityTypeOtherNavigation = @"OtherNavigation";

@interface MGLConfigurationManager ()

@property (atomic) BOOL mapboxMetricsEnabledSettingShownInApp;
@property (atomic) NSString *accessToken;
@property (atomic) CLActivityType activityType;

@end


@implementation MGLConfigurationManager

+ (void)load {
    // Read the initial configuration from Info.plist. The shown-in-app setting
    // preempts the Settings bundle check in -[MGLMapboxEvents init] triggered
    // by setting the access token.
    NSBundle *bundle = [NSBundle mainBundle];
    
    NSNumber *shownInAppNumber = [bundle objectForInfoDictionaryKey:kMGLMapboxMetricsEnabledSettingShownInApp];
    if (shownInAppNumber) {
        [MGLConfigurationManager sharedManager].mapboxMetricsEnabledSettingShownInApp = [shownInAppNumber boolValue];
    }
    
    NSString *accessToken = [bundle objectForInfoDictionaryKey:kMGLMapboxAccessToken];
    if (accessToken.length) {
        self.accessToken = accessToken;
    }
    
    NSString *activityType = [bundle objectForInfoDictionaryKey:kMGLMapboxActivityType];
    if (activityType.length) {
        self.activityType = [self activityTypeForBundleStringValue:activityType];
    }
}

// Can be called from any thread.
//
+ (instancetype) sharedManager {
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
    static dispatch_once_t onceToken;
    static MGLConfigurationManager *_sharedManager;
    void (^setupBlock)() = ^{
        dispatch_once(&onceToken, ^{
            _sharedManager = [[self alloc] init];
        });
    };
    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            setupBlock();
        });
    }
    else {
        setupBlock();
    }
    return _sharedManager;
}

+ (BOOL) mapboxMetricsEnabledSettingShownInApp {
    return [MGLConfigurationManager sharedManager].mapboxMetricsEnabledSettingShownInApp;
}

+ (void) setAccessToken:(NSString *) accessToken {
    if ( ! [accessToken length]) return;
    
    [MGLConfigurationManager sharedManager].accessToken = accessToken;

    // Update MGLMapboxEvents
    // NOTE: This is (likely) the initial setup of MGLMapboxEvents
    [MGLMapboxEvents sharedManager];
}

+ (NSString *) accessToken {
    return [MGLConfigurationManager sharedManager].accessToken;
}

+ (void) setActivityType:(CLActivityType)activityType {
    [MGLConfigurationManager sharedManager].activityType = activityType;
    
    // Ensure MGLMapboxEvents configures location manager with new activity type
    [MGLMapboxEvents validate];
}

+ (CLActivityType) activityType {
    return [MGLConfigurationManager sharedManager].activityType;
}

#pragma mark - Utility

+ (CLActivityType)activityTypeForBundleStringValue:(NSString *)stringValue {
    if ([stringValue isEqualToString:kMGLMapboxActivityTypeAutomotiveNavigation]) {
        return CLActivityTypeAutomotiveNavigation;
    }
    if ([stringValue isEqual:kMGLMapboxActivityTypeFitness]) {
        return CLActivityTypeFitness;
    }
    if ([stringValue isEqual:kMGLMapboxActivityTypeOtherNavigation]) {
        return CLActivityTypeOtherNavigation;
    }
    return CLActivityTypeOther;
}


@end