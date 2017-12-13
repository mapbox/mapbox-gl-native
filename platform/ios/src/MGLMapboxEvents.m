#import "MGLMapboxEvents.h"
#import "NSBundle+MGLAdditions.h"

static NSString * const MGLAPIClientUserAgentBase = @"MapboxEventsiOS";

@interface MGLMapboxEvents ()

@property (nonatomic) MMEEventsManager *eventsManager;

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

+ (nullable instancetype)sharedManager {
    static dispatch_once_t onceToken;
    static MGLMapboxEvents *_sharedManager;
    dispatch_once(&onceToken, ^{
        _sharedManager = [[self alloc] init];
    });
    return _sharedManager;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _eventsManager = [[MMEEventsManager alloc] init];
        _eventsManager.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"];
        _eventsManager.metricsEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userDefaultsDidChange:) name:NSUserDefaultsDidChangeNotification object:nil];
    }
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)userDefaultsDidChange:(NSNotification *)notification {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.eventsManager.debugLoggingEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"];
        self.eventsManager.metricsEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"];
        [self.eventsManager pauseOrResumeMetricsCollectionIfRequired];
    });
}

+ (void)setupWithAccessToken:(NSString *)accessToken {
    NSString *semanticVersion = [NSBundle mgl_frameworkInfoDictionary][@"MGLSemanticVersionString"];
    NSString *shortVersion = [NSBundle mgl_frameworkInfoDictionary][@"CFBundleShortVersionString"];
    NSString *sdkVersion = semanticVersion ?: shortVersion;
    [[[self sharedManager] eventsManager] initializeWithAccessToken:accessToken userAgentBase:MGLAPIClientUserAgentBase hostSDKVersion:sdkVersion];
}

+ (void)pushTurnstileEvent {
    [[[self sharedManager] eventsManager] sendTurnstileEvent];
}

+ (void)pushEvent:(NSString *)event withAttributes:(MMEMapboxEventAttributes *)attributeDictionary {
    [[[self sharedManager] eventsManager] enqueueEventWithName:event attributes:attributeDictionary];
}

+ (void)flush {
    [[[self sharedManager] eventsManager] flush];
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
