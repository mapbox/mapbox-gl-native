#import "MGLMapboxEvents.h"
#import "MBXSKUToken.h"
#import "NSBundle+MGLAdditions.h"
#import "MGLAccountManager_Private.h"

static NSString * const MGLAPIClientUserAgentBase = @"mapbox-maps-ios";
static NSString * const MGLMapboxAccountType = @"MGLMapboxAccountType";
static NSString * const MGLMapboxMetricsEnabled = @"MGLMapboxMetricsEnabled";
static NSString * const MGLMapboxMetricsDebugLoggingEnabled = @"MGLMapboxMetricsDebugLoggingEnabled";
static NSString * const MGLTelemetryAccessToken = @"MGLTelemetryAccessToken";
static NSString * const MGLTelemetryBaseURL = @"MGLTelemetryBaseURL";
static NSString * const MGLEventsProfile = @"MMEEventsProfile";
static NSString * const MGLVariableGeofence = @"VariableGeofence";

@interface MGLMapboxEvents ()

@property (nonatomic) MMEEventsManager *eventsManager;
@property (nonatomic) NSURL *baseURL;
@property (nonatomic, copy) NSString *accessToken;

@end
@implementation MGLMapboxEvents


+ (void)initialize {
    if (self == [MGLMapboxEvents class]) {
        NSBundle *bundle = [NSBundle mainBundle];
        NSNumber *accountTypeNumber = [bundle objectForInfoDictionaryKey:MGLMapboxAccountType];
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{MGLMapboxAccountType: accountTypeNumber ?: @0,
                                                                  MGLMapboxMetricsEnabled: @YES,
                                                                  MGLMapboxMetricsDebugLoggingEnabled: @NO}];
    }
}

+ (nullable instancetype)sharedInstance {
    
    static dispatch_once_t onceToken;
    static MGLMapboxEvents *_sharedInstance;
    dispatch_once(&onceToken, ^{
        _sharedInstance = [[self alloc] init];
    });
    return _sharedInstance;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _eventsManager = MMEEventsManager.sharedManager;
        _eventsManager.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:MGLMapboxMetricsDebugLoggingEnabled];
        _eventsManager.accountType = [[NSUserDefaults standardUserDefaults] integerForKey:MGLMapboxAccountType];
        _eventsManager.metricsEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:MGLMapboxMetricsEnabled];
        
        // It is possible for the shared instance of this class to be created because of a call to
        // +[MGLAccountManager load] early on in the app lifecycle of the host application.
        // If user default values for access token and base URL are available, they are stored here
        // on local properties so that they can be applied later once MMEEventsManager is fully initialized
        // (once -[MMEEventsManager initializeWithAccessToken:userAgentBase:hostSDKVersion:] is called.
        // Normally, the telem access token and base URL are not set this way. However, overriding these values
        // with user defaults can be useful for testing with an alternative (test) backend system.
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:MGLTelemetryAccessToken]) {
            self.accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:MGLTelemetryAccessToken];
        }
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:MGLTelemetryBaseURL]) {
            self.baseURL = [NSURL URLWithString:[[NSUserDefaults standardUserDefaults] objectForKey:MGLTelemetryBaseURL]];
        }
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userDefaultsDidChange:) name:NSUserDefaultsDidChangeNotification object:nil];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)userDefaultsDidChange:(NSNotification *)notification {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self updateNonDisablingConfigurationValues];
        [self updateDisablingConfigurationValuesWithNotification:notification];
    });
}

- (void)updateNonDisablingConfigurationValues {
    self.eventsManager.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"];
    
    // It is possible for `MGLTelemetryAccessToken` to have been set yet `userDefaultsDidChange:`
    // is called before `setupWithAccessToken:` is called.
    // In that case, setting the access token here will have no effect. In practice, that's fine
    // because the access token value will be resolved when `setupWithAccessToken:` is called eventually
    if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:MGLTelemetryAccessToken]) {
        self.eventsManager.accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:MGLTelemetryAccessToken];
    }
    
    // It is possible for `MGLTelemetryBaseURL` to have been set yet `userDefaultsDidChange:`
    // is called before setupWithAccessToken: is called.
    // In that case, setting the base URL here will have no effect. In practice, that's fine
    // because the base URL value will be resolved when `setupWithAccessToken:` is called eventually
    if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:MGLTelemetryBaseURL]) {
        NSURL *baseURL = [NSURL URLWithString:[[NSUserDefaults standardUserDefaults] objectForKey:MGLTelemetryBaseURL]];
        self.eventsManager.baseURL = baseURL;
    }
}

- (void)updateDisablingConfigurationValuesWithNotification:(NSNotification *)notification {
    // Guard against over calling pause / resume if the values this implementation actually
    // cares about have not changed. We guard because the pause and resume method checks CoreLocation's
    // authorization status and that can drag on the main thread if done too many times (e.g. if the host
    // app heavily uses the user defaults API and this method is called very frequently)
    if ([[notification object] respondsToSelector:@selector(objectForKey:)]) {
        NSUserDefaults *userDefaults = [notification object];
        
        NSInteger accountType = [userDefaults integerForKey:MGLMapboxAccountType];
        BOOL metricsEnabled = [userDefaults boolForKey:MGLMapboxMetricsEnabled];
        
        if (accountType != self.eventsManager.accountType || metricsEnabled != self.eventsManager.metricsEnabled) {
            self.eventsManager.accountType = accountType;
            self.eventsManager.metricsEnabled = metricsEnabled;
            
            [self.eventsManager pauseOrResumeMetricsCollectionIfRequired];
        }
    }
}

+ (void)setupWithAccessToken:(NSString *)accessToken {
    int64_t delayTime = 0;
    
    if ([[[NSBundle mainBundle] objectForInfoDictionaryKey:MGLEventsProfile] isEqualToString:MGLVariableGeofence]) {
        delayTime = 10;
    }
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayTime * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSString *semanticVersion = [NSBundle mgl_frameworkInfoDictionary][@"MGLSemanticVersionString"];
        NSString *shortVersion = [NSBundle mgl_frameworkInfoDictionary][@"CFBundleShortVersionString"];
        NSString *sdkVersion = semanticVersion ?: shortVersion;
        
        // It is possible that an alternative access token was already set on this instance when the class was loaded
        // Use it if it exists
        NSString *resolvedAccessToken = [MGLMapboxEvents sharedInstance].accessToken ?: accessToken;
        
        [[[self sharedInstance] eventsManager] initializeWithAccessToken:resolvedAccessToken userAgentBase:MGLAPIClientUserAgentBase hostSDKVersion:sdkVersion];
        
        // It is possible that an alternative base URL was set on this instance when the class was loaded
        // Use it if it exists
        if ([MGLMapboxEvents sharedInstance].baseURL) {
            [[MGLMapboxEvents sharedInstance] eventsManager].baseURL = [MGLMapboxEvents sharedInstance].baseURL;
        }

        [[self sharedInstance] eventsManager].skuId = MBXAccountsSKUIDMaps;
        
        [self flush];
    });
}

+ (void)pushTurnstileEvent {
    [[[self sharedInstance] eventsManager] sendTurnstileEvent];
}

+ (void)pushEvent:(NSString *)event withAttributes:(MMEMapboxEventAttributes *)attributeDictionary {
    [[[self sharedInstance] eventsManager] enqueueEventWithName:event attributes:attributeDictionary];
}

+ (void)flush {
    [[[self sharedInstance] eventsManager] flush];
}

+ (void)ensureMetricsOptoutExists {
    NSNumber *shownInAppNumber = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLMapboxMetricsEnabledSettingShownInApp"];
    BOOL metricsEnabledSettingShownInAppFlag = [shownInAppNumber boolValue];
    
    if (!metricsEnabledSettingShownInAppFlag &&
        [[NSUserDefaults standardUserDefaults] integerForKey:MGLMapboxAccountType] == 0) {
        // Opt-out is not configured in UI, so check for Settings.bundle
        id defaultEnabledValue;
        NSString *appSettingsBundle = [[NSBundle mainBundle] pathForResource:@"Settings" ofType:@"bundle"];
        
        if (appSettingsBundle) {
            // Dynamic Settings.bundle loading based on http://stackoverflow.com/a/510329/2094275
            NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[appSettingsBundle stringByAppendingPathComponent:@"Root.plist"]];
            NSArray *preferences = settings[@"PreferenceSpecifiers"];
            for (NSDictionary *prefSpecification in preferences) {
                if ([prefSpecification[@"Key"] isEqualToString:MGLMapboxMetricsEnabled]) {
                    defaultEnabledValue = prefSpecification[@"DefaultValue"];
                }
            }
        }
        
        if (!defaultEnabledValue) {
            [NSException raise:@"Telemetry opt-out missing" format:
             @"End users must be able to opt out of Mapbox Telemetry in your app, either inside Settings (via Settings.bundle) or inside this app. "
             @"By default, this opt-out control is included as a menu item in the attribution action sheet. "
             @"If you reimplement the opt-out control inside this app, disable this assertion by setting MGLMapboxMetricsEnabledSettingShownInApp to YES in Info.plist."
             @"\n\nSee https://docs.mapbox.com/help/how-mapbox-works/attribution/#mapbox-maps-sdk-for-ios for more information."
             @"\n\nAdditionally, by hiding this attribution control you agree to display the required attribution elsewhere in this app."];
        }
    }
}

@end
