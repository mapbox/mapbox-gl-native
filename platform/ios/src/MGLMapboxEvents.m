#import "MGLMapboxEvents.h"
#import "NSBundle+MGLAdditions.h"
#import "NSProcessInfo+MGLAdditions.h"

static NSString * const MGLAPIClientUserAgentBase = @"MapboxEventsiOS";

@interface MGLMapboxEvents ()

@property (nonatomic) MMEEventsManager *eventsManager;
@property (nonatomic) NSURL *baseURL;
@property (nonatomic, copy) NSString *accessToken;

@end
@implementation MGLMapboxEvents


+ (void)initialize {
    if (self == [MGLMapboxEvents class]) {
        NSBundle *bundle = [NSBundle mainBundle];
        NSNumber *accountTypeNumber = [bundle objectForInfoDictionaryKey:@"MGLMapboxAccountType"];
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{@"MGLMapboxAccountType": accountTypeNumber ?: @0,
                                                                  @"MGLMapboxMetricsEnabled": @YES,
                                                                  @"MGLMapboxMetricsDebugLoggingEnabled": @NO}];
    }
}

+ (nullable instancetype)sharedInstance {
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
    
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
        _eventsManager = [[MMEEventsManager alloc] init];
        _eventsManager.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"];
        _eventsManager.accountType = [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"];
        _eventsManager.metricsEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"];
        
        // Because we can get here [MGLAccountManaber +load] it is possible that values from user defaults will be read and
        // applied here. These checks and local accessToken and baseURL assigns work around that fact. If user default value
        // are set, they are stored on the local properties here and then only applied later on once MMEEventsManager is
        // fully initialized (once -[MMEEventsManager initializeWithAccessToken:userAgentBase:hostSDKVersion:] is called.
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:@"MGLTelemetryAccessToken"]) {
            self.accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:@"MGLTelemetryAccessToken"];
        }
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:@"MGLTelemetryBaseURL"]) {
            self.baseURL = [NSURL URLWithString:[[NSUserDefaults standardUserDefaults] objectForKey:@"MGLTelemetryBaseURL"]];
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
        self.eventsManager.metricsEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"];
        self.eventsManager.accountType = [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"];
        self.eventsManager.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"];                
        

        // It is possible that MGLTelemetryAccessToken is set and userDefaultsDidChange: is called before setupWithAccessToken: is called.
        // In that case, setting the access token here will be a noop. In practice, that's fine because the access token value
        // will be resolved when setupWithAccessToken: is called eventually
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:@"MGLTelemetryAccessToken"]) {
            self.eventsManager.accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:@"MGLTelemetryAccessToken"];
        }
        
        // It is possible that MGLTelemetryBaseURL is set and userDefaultsDidChange: is called before setupWithAccessToken: is called.
        // In that case, setting baseURL here will be a noop. In practice, that's fine because the baseURL value
        // will be resolved when setupWithAccessToken: is called eventually
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:@"MGLTelemetryBaseURL"]) {
            NSURL *baseURL = [NSURL URLWithString:[[NSUserDefaults standardUserDefaults] objectForKey:@"MGLTelemetryBaseURL"]];
            self.eventsManager.baseURL = baseURL;
        }
        
        [self.eventsManager pauseOrResumeMetricsCollectionIfRequired];
    });
}

+ (void)setupWithAccessToken:(NSString *)accessToken {
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
        [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"] == 0) {
        // Opt-out is not configured in UI, so check for Settings.bundle
        id defaultEnabledValue;
        NSString *appSettingsBundle = [[NSBundle mainBundle] pathForResource:@"Settings" ofType:@"bundle"];
        
        if (appSettingsBundle) {
            // Dynamic Settings.bundle loading based on http://stackoverflow.com/a/510329/2094275
            NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[appSettingsBundle stringByAppendingPathComponent:@"Root.plist"]];
            NSArray *preferences = settings[@"PreferenceSpecifiers"];
            for (NSDictionary *prefSpecification in preferences) {
                if ([prefSpecification[@"Key"] isEqualToString:@"MGLMapboxMetricsEnabled"]) {
                    defaultEnabledValue = prefSpecification[@"DefaultValue"];
                }
            }
        }
        
        if (!defaultEnabledValue) {
            [NSException raise:@"Telemetry opt-out missing" format:
             @"End users must be able to opt out of Mapbox Telemetry in your app, either inside Settings (via Settings.bundle) or inside this app. "
             @"By default, this opt-out control is included as a menu item in the attribution action sheet. "
             @"If you reimplement the opt-out control inside this app, disable this assertion by setting MGLMapboxMetricsEnabledSettingShownInApp to YES in Info.plist."
             @"\n\nSee https://www.mapbox.com/ios-sdk/#telemetry_opt_out for more information."
             @"\n\nAdditionally, by hiding this attribution control you agree to display the required attribution elsewhere in this app."];
        }
    }
}

@end
