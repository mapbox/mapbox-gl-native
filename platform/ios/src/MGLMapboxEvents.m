#import "MGLMapboxEvents.h"
#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import "MGLAccountManager.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSException+MGLAdditions.h"
#import "MGLAPIClient.h"
#import "MGLLocationManager.h"
#import "MGLTelemetryConfig.h"

#include <mbgl/storage/reachability.h>
#include <sys/sysctl.h>

// Event types
NSString *const MGLEventTypeAppUserTurnstile = @"appUserTurnstile";
NSString *const MGLEventTypeMapLoad = @"map.load";
NSString *const MGLEventTypeMapTap = @"map.click";
NSString *const MGLEventTypeMapDragEnd = @"map.dragend";
NSString *const MGLEventTypeLocation = @"location";
NSString *const MGLEventTypeLocalDebug = @"debug";

// Gestures
NSString *const MGLEventGestureSingleTap = @"SingleTap";
NSString *const MGLEventGestureDoubleTap = @"DoubleTap";
NSString *const MGLEventGestureTwoFingerSingleTap = @"TwoFingerTap";
NSString *const MGLEventGestureQuickZoom = @"QuickZoom";
NSString *const MGLEventGesturePanStart = @"Pan";
NSString *const MGLEventGesturePinchStart = @"Pinch";
NSString *const MGLEventGestureRotateStart = @"Rotation";
NSString *const MGLEventGesturePitchStart = @"Pitch";

// Event keys
NSString *const MGLEventKeyLatitude = @"lat";
NSString *const MGLEventKeyLongitude = @"lng";
NSString *const MGLEventKeyZoomLevel = @"zoom";
NSString *const MGLEventKeySpeed = @"speed";
NSString *const MGLEventKeyCourse = @"course";
NSString *const MGLEventKeyGestureID = @"gesture";
NSString *const MGLEventHorizontalAccuracy = @"horizontalAccuracy";
NSString *const MGLEventKeyLocalDebugDescription = @"debug.description";

static NSString *const MGLEventKeyEvent = @"event";
static NSString *const MGLEventKeyCreated = @"created";
static NSString *const MGLEventKeyVendorID = @"userId";
static NSString *const MGLEventKeyModel = @"model";
static NSString *const MGLEventKeyEnabledTelemetry = @"enabled.telemetry";
static NSString *const MGLEventKeyOperatingSystem = @"operatingSystem";
static NSString *const MGLEventKeyResolution = @"resolution";
static NSString *const MGLEventKeyAccessibilityFontScale = @"accessibilityFontScale";
static NSString *const MGLEventKeyOrientation = @"orientation";
static NSString *const MGLEventKeyPluggedIn = @"pluggedIn";
static NSString *const MGLEventKeyWifi = @"wifi";
static NSString *const MGLEventKeySource = @"source";
static NSString *const MGLEventKeySessionId = @"sessionId";
static NSString *const MGLEventKeyApplicationState = @"applicationState";
static NSString *const MGLEventKeyAltitude = @"altitude";

static NSString *const MGLMapboxAccountType = @"MGLMapboxAccountType";
static NSString *const MGLMapboxMetricsEnabled = @"MGLMapboxMetricsEnabled";

// SDK event source
static NSString *const MGLEventSource = @"mapbox";

// Event application state
static NSString *const MGLApplicationStateForeground = @"Foreground";
static NSString *const MGLApplicationStateBackground = @"Background";
static NSString *const MGLApplicationStateInactive = @"Inactive";
static NSString *const MGLApplicationStateUnknown = @"Unknown";

const NSUInteger MGLMaximumEventsPerFlush = 180;
const NSTimeInterval MGLFlushInterval = 180;

@interface MGLMapboxEventsData : NSObject

@property (nonatomic) NSString *vendorId;
@property (nonatomic) NSString *model;
@property (nonatomic) NSString *iOSVersion;
@property (nonatomic) CGFloat scale;

@end

@implementation MGLMapboxEventsData

- (instancetype)init {
    if (self = [super init]) {
        _vendorId = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        _model = [self sysInfoByName:"hw.machine"];
        _iOSVersion = [NSString stringWithFormat:@"%@ %@", [UIDevice currentDevice].systemName, [UIDevice currentDevice].systemVersion];
        if ([UIScreen instancesRespondToSelector:@selector(nativeScale)]) {
            _scale = [UIScreen mainScreen].nativeScale;
        } else {
            _scale = [UIScreen mainScreen].scale;
        }
    }
    return self;
}

- (NSString *)sysInfoByName:(char *)typeSpecifier {
    size_t size;
    sysctlbyname(typeSpecifier, NULL, &size, NULL, 0);

    char *answer = malloc(size);
    sysctlbyname(typeSpecifier, answer, &size, NULL, 0);

    NSString *results = [NSString stringWithCString:answer encoding: NSUTF8StringEncoding];

    free(answer);
    return results;
}

@end

@interface MGLMapboxEvents () <MGLLocationManagerDelegate>

@property (nonatomic) MGLMapboxEventsData *data;
@property (nonatomic, copy) NSString *appBundleId;
@property (nonatomic, readonly) NSString *instanceID;
@property (nonatomic, copy) NSString *dateForDebugLogFile;
@property (nonatomic) NSDateFormatter *rfc3339DateFormatter;
@property (nonatomic) MGLAPIClient *apiClient;
@property (nonatomic) BOOL usesTestServer;
@property (nonatomic) BOOL canEnableDebugLogging;
@property (nonatomic, getter=isPaused) BOOL paused;
@property (nonatomic) NS_MUTABLE_ARRAY_OF(MGLMapboxEventAttributes *) *eventQueue;
@property (nonatomic) dispatch_queue_t serialQueue;
@property (nonatomic) dispatch_queue_t debugLogSerialQueue;
@property (nonatomic) MGLLocationManager *locationManager;
@property (nonatomic) NSTimer *timer;
@property (nonatomic) NSDate *instanceIDRotationDate;
@property (nonatomic) NSDate *nextTurnstileSendDate;
@property (nonatomic) NSNumber *currentAccountTypeValue;
@property (nonatomic) BOOL currentMetricsEnabledValue;

@end

@implementation MGLMapboxEvents {
    NSString *_instanceID;
    UIBackgroundTaskIdentifier _backgroundTaskIdentifier;
}

+ (void)initialize {
    if (self == [MGLMapboxEvents class]) {
        NSBundle *bundle = [NSBundle mainBundle];
        NSNumber *accountTypeNumber = [bundle objectForInfoDictionaryKey:MGLMapboxAccountType];
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{
             MGLMapboxAccountType: accountTypeNumber ?: @0,
             MGLMapboxMetricsEnabled: @YES,
             @"MGLMapboxMetricsDebugLoggingEnabled": @NO,
         }];
    }
}

+ (BOOL)isEnabled {
#if TARGET_OS_SIMULATOR
    return NO;
#else
    BOOL isLowPowerModeEnabled = NO;
    if ([NSProcessInfo instancesRespondToSelector:@selector(isLowPowerModeEnabled)]) {
        isLowPowerModeEnabled = [[NSProcessInfo processInfo] isLowPowerModeEnabled];
    }
    return ([[NSUserDefaults standardUserDefaults] boolForKey:MGLMapboxMetricsEnabled] &&
            [[NSUserDefaults standardUserDefaults] integerForKey:MGLMapboxAccountType] == 0 &&
            !isLowPowerModeEnabled);
#endif
}


- (BOOL)debugLoggingEnabled {
    return (self.canEnableDebugLogging &&
            [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"]);
}

- (instancetype) init {
    self = [super init];
    if (self) {
        [MGLTelemetryConfig.sharedConfig configurationFromKey:[[NSUserDefaults standardUserDefaults] objectForKey:MGLMapboxMetricsProfile]];
        
        _currentAccountTypeValue = @0;
        _currentMetricsEnabledValue = YES;
        
        _appBundleId = [[NSBundle mainBundle] bundleIdentifier];
        _apiClient = [[MGLAPIClient alloc] init];

        NSString *uniqueID = [[NSProcessInfo processInfo] globallyUniqueString];
        _serialQueue = dispatch_queue_create([[NSString stringWithFormat:@"%@.%@.events.serial", _appBundleId, uniqueID] UTF8String], DISPATCH_QUEUE_SERIAL);

        _locationManager = [[MGLLocationManager alloc] init];
        _locationManager.delegate = self;
        _paused = YES;
        [self resumeMetricsCollection];

        // Events Control
        _eventQueue = [[NSMutableArray alloc] init];

        // Setup Date Format
        _rfc3339DateFormatter = [[NSDateFormatter alloc] init];
        NSLocale *enUSPOSIXLocale = [[NSLocale alloc] initWithLocaleIdentifier:@"en_US_POSIX"];

        [_rfc3339DateFormatter setLocale:enUSPOSIXLocale];
        [_rfc3339DateFormatter setDateFormat:@"yyyy'-'MM'-'dd'T'HH':'mm':'ssZ"];
        // Clear Any System TimeZone Cache
        [NSTimeZone resetSystemTimeZone];
        [_rfc3339DateFormatter setTimeZone:[NSTimeZone systemTimeZone]];

        // Configure logging
        if ([self isProbablyAppStoreBuild]) {
            self.canEnableDebugLogging = NO;

            if ([[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"]) {
                NSLog(@"Telemetry logging is only enabled in non-app store builds.");
            }
        } else {
            self.canEnableDebugLogging = YES;
        }

        // Watch for changes to telemetry settings by the user
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userDefaultsDidChange:) name:NSUserDefaultsDidChangeNotification object:nil];

        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(pauseOrResumeMetricsCollectionIfRequired) name:UIApplicationDidEnterBackgroundNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(pauseOrResumeMetricsCollectionIfRequired) name:UIApplicationDidBecomeActiveNotification object:nil];

        // Watch for Low Power Mode change events
        if (&NSProcessInfoPowerStateDidChangeNotification != NULL) {
            [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(pauseOrResumeMetricsCollectionIfRequired) name:NSProcessInfoPowerStateDidChangeNotification object:nil];
        }
    }
    return self;
}

// Called implicitly from any public class convenience methods.
// May return nil if this feature is disabled.
//
+ (nullable instancetype)sharedManager {
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
    static dispatch_once_t onceToken;
    static MGLMapboxEvents *_sharedManager;
    dispatch_once(&onceToken, ^{
        _sharedManager = [[self alloc] init];
    });
    return _sharedManager;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [self pauseMetricsCollection];
}

- (NSString *)instanceID {
    if (self.instanceIDRotationDate && [[NSDate date] timeIntervalSinceDate:self.instanceIDRotationDate] >= 0) {
        _instanceID = nil;
    }
    if (!_instanceID) {
        _instanceID = [[NSUUID UUID] UUIDString];
        NSTimeInterval twentyFourHourTimeInterval = 24 * 3600;
        self.instanceIDRotationDate = [[NSDate date] dateByAddingTimeInterval:twentyFourHourTimeInterval];
    }
    return _instanceID;
}

- (void)userDefaultsDidChange:(NSNotification *)notification {
    
    // Guard against over calling pause / resume if the values this implementation actually
    // cares about have not changed
    
    if ([[notification object] respondsToSelector:@selector(objectForKey:)]) {
        NSUserDefaults *userDefaults = [notification object];
        
        NSNumber *accountType = [userDefaults objectForKey:MGLMapboxAccountType];
        BOOL metricsEnabled = [[userDefaults objectForKey:MGLMapboxMetricsEnabled] boolValue];
        
        if (![accountType isEqualToNumber:self.currentAccountTypeValue] || metricsEnabled != self.currentMetricsEnabledValue) {
            [self pauseOrResumeMetricsCollectionIfRequired];
            self.currentAccountTypeValue = accountType;
            self.currentMetricsEnabledValue = metricsEnabled;
        }        
    }
    
}

- (void)pauseOrResumeMetricsCollectionIfRequired {
    
    // [CLLocationManager authorizationStatus] has been found to block in some cases so
    // dispatch the call to a non-UI thread
    dispatch_async(self.serialQueue, ^{
        CLAuthorizationStatus status = [CLLocationManager authorizationStatus];
        
        // Checking application state must be done on the main thread for safety and
        // to avoid a thread sanitizer error
        dispatch_async(dispatch_get_main_queue(), ^{
            UIApplication *application = [UIApplication sharedApplication];
            UIApplicationState state = application.applicationState;
            
            // Prevent blue status bar when host app has `when in use` permission only and it is not in foreground
            if (status == kCLAuthorizationStatusAuthorizedWhenInUse && state == UIApplicationStateBackground) {
                if (_backgroundTaskIdentifier == UIBackgroundTaskInvalid) {
                    _backgroundTaskIdentifier = [application beginBackgroundTaskWithExpirationHandler:^{
                        [application endBackgroundTask:_backgroundTaskIdentifier];
                        _backgroundTaskIdentifier = UIBackgroundTaskInvalid;
                    }];
                    [self flush];
                }
                [self pauseMetricsCollection];
                return;
            }
            
            // Toggle pause based on current pause state, user opt-out state, and low-power state.
            BOOL enabled = [[self class] isEnabled];
            if (self.paused && enabled) {
                [self resumeMetricsCollection];
            } else if (!self.paused && !enabled) {
                [self flush];
                [self pauseMetricsCollection];
            }
        });
    });
}

- (void)pauseMetricsCollection {
    if (self.paused) {
        return;
    }

    self.paused = YES;
    [self.timer invalidate];
    self.timer = nil;
    [self.eventQueue removeAllObjects];
    self.data = nil;

    [self.locationManager stopUpdatingLocation];
}

- (void)resumeMetricsCollection {
    if (!self.paused || ![[self class] isEnabled]) {
        return;
    }

    self.paused = NO;
    self.data = [[MGLMapboxEventsData alloc] init];

    [self.locationManager startUpdatingLocation];
}

+ (void)flush {
    [[MGLMapboxEvents sharedManager] flush];
}

- (void)flush {
    if ([MGLAccountManager accessToken] == nil) {
        return;
    }

    NSArray *events = [NSArray arrayWithArray:self.eventQueue];
    [self.eventQueue removeAllObjects];

    [self postEvents:events];

    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }

    [self pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription:@"flush"}];
}

- (void)pushTurnstileEvent {
    if (self.nextTurnstileSendDate && [[NSDate date] timeIntervalSinceDate:self.nextTurnstileSendDate] < 0) {
        return;
    }

    NSString *vendorID = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
    if (!vendorID) {
        return;
    }

    NSDictionary *turnstileEventAttributes = @{MGLEventKeyEvent: MGLEventTypeAppUserTurnstile,
                                               MGLEventKeyCreated: [self.rfc3339DateFormatter stringFromDate:[NSDate date]],
                                               MGLEventKeyVendorID: vendorID,
                                               MGLEventKeyEnabledTelemetry: @([[self class] isEnabled])};

    if ([MGLAccountManager accessToken] == nil) {
        return;
    }

    __weak __typeof__(self) weakSelf = self;
    [self.apiClient postEvent:turnstileEventAttributes completionHandler:^(NSError * _Nullable error) {
        __strong __typeof__(weakSelf) strongSelf = weakSelf;
        if (error) {
            [strongSelf pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription: @"Network error",
                                                                         @"error": error}];
            return;
        }
        [strongSelf writeEventToLocalDebugLog:turnstileEventAttributes];
        [strongSelf updateNextTurnstileSendDate];
    }];
}

- (void)updateNextTurnstileSendDate {
    // Find the time a day from now (sometime tomorrow)
    NSCalendar *calendar = [[NSCalendar alloc] initWithCalendarIdentifier:NSCalendarIdentifierGregorian];
    NSDateComponents *dayComponent = [[NSDateComponents alloc] init];
    dayComponent.day = 1;
    NSDate *sometimeTomorrow = [calendar dateByAddingComponents:dayComponent toDate:[NSDate date] options:0];

    // Find the start of tomorrow and use that as the next turnstile send date. The effect of this is that
    // turnstile events can be sent as much as once per calendar day and always at the start of a session
    // when a map load happens.
    NSDate *startOfTomorrow = nil;
    [calendar rangeOfUnit:NSCalendarUnitDay startDate:&startOfTomorrow interval:nil forDate:sometimeTomorrow];
    self.nextTurnstileSendDate = startOfTomorrow;
}

+ (void)pushEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    [[MGLMapboxEvents sharedManager] pushEvent:event withAttributes:attributeDictionary];
}

- (void)pushEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    if (!event) {
        return;
    }

    if ([event isEqualToString:MGLEventTypeMapLoad]) {
        [self pushTurnstileEvent];
    }

    if (self.paused) {
        return;
    }

    MGLMapboxEventAttributes *fullyFormedEvent = [self fullyFormedEventForEvent:event withAttributes:attributeDictionary];
    if (fullyFormedEvent) {
        [self.eventQueue addObject:fullyFormedEvent];
        [self writeEventToLocalDebugLog:fullyFormedEvent];
        // Has Flush Limit Been Reached?
        if (self.eventQueue.count >= MGLMaximumEventsPerFlush) {
            [self flush];
        } else if (self.eventQueue.count ==  1) {
            // If this is first new event on queue start timer,
            [self startTimer];
        }
    } else {
        [self pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription: @"Unknown event",
                                                                     @"eventName": event,
                                                                     @"event.attributes": attributeDictionary}];
    }
}

#pragma mark Events

- (MGLMapboxEventAttributes *)fullyFormedEventForEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    if ([event isEqualToString:MGLEventTypeMapLoad]) {
        return  [self mapLoadEventWithAttributes:attributeDictionary];
    } else if ([event isEqualToString:MGLEventTypeMapTap]) {
        return [self mapClickEventWithAttributes:attributeDictionary];
    } else if ([event isEqualToString:MGLEventTypeMapDragEnd]) {
        return [self mapDragEndEventWithAttributes:attributeDictionary];
    } else if ([event isEqualToString:MGLEventTypeLocation]) {
        return [self locationEventWithAttributes:attributeDictionary];
    }
    return nil;
}

- (MGLMapboxEventAttributes *)locationEventWithAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    MGLMutableMapboxEventAttributes *attributes = [NSMutableDictionary dictionary];
    attributes[MGLEventKeyEvent] = MGLEventTypeLocation;
    attributes[MGLEventKeySource] = MGLEventSource;
    attributes[MGLEventKeySessionId] = self.instanceID;
    attributes[MGLEventKeyOperatingSystem] = self.data.iOSVersion;
    NSString *currentApplicationState = [self applicationState];
    if (![currentApplicationState isEqualToString:MGLApplicationStateUnknown]) {
        attributes[MGLEventKeyApplicationState] = currentApplicationState;
    }

    return [self eventForAttributes:attributes attributeDictionary:attributeDictionary];
}

- (MGLMapboxEventAttributes *)mapLoadEventWithAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    MGLMutableMapboxEventAttributes *attributes = [NSMutableDictionary dictionary];
    attributes[MGLEventKeyEvent] = MGLEventTypeMapLoad;
    attributes[MGLEventKeyCreated] = [self.rfc3339DateFormatter stringFromDate:[NSDate date]];
    attributes[MGLEventKeyVendorID] = self.data.vendorId;
    attributes[MGLEventKeyModel] = self.data.model;
    attributes[MGLEventKeyOperatingSystem] = self.data.iOSVersion;
    attributes[MGLEventKeyResolution] = @(self.data.scale);
    attributes[MGLEventKeyAccessibilityFontScale] = @([self contentSizeScale]);
    attributes[MGLEventKeyOrientation] = [self deviceOrientation];
    attributes[MGLEventKeyWifi] = @([[MGLReachability reachabilityForLocalWiFi] isReachableViaWiFi]);

    return [self eventForAttributes:attributes attributeDictionary:attributeDictionary];
}

- (MGLMapboxEventAttributes *)mapClickEventWithAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    MGLMutableMapboxEventAttributes *attributes = [self interactionEvent];
    attributes[MGLEventKeyEvent] = MGLEventTypeMapTap;
    return [self eventForAttributes:attributes attributeDictionary:attributeDictionary];
}

- (MGLMapboxEventAttributes *)mapDragEndEventWithAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    MGLMutableMapboxEventAttributes *attributes = [self interactionEvent];
    attributes[MGLEventKeyEvent] = MGLEventTypeMapDragEnd;

    return [self eventForAttributes:attributes attributeDictionary:attributeDictionary];
}

- (MGLMutableMapboxEventAttributes *)interactionEvent {
    MGLMutableMapboxEventAttributes *attributes = [NSMutableDictionary dictionary];
    attributes[MGLEventKeyCreated] = [self.rfc3339DateFormatter stringFromDate:[NSDate date]];
    attributes[MGLEventKeyOrientation] = [self deviceOrientation];
    attributes[MGLEventKeyWifi] = @([[MGLReachability reachabilityForLocalWiFi] isReachableViaWiFi]);

    return attributes;
}

- (MGLMapboxEventAttributes *)eventForAttributes:(MGLMutableMapboxEventAttributes *)attributes attributeDictionary:(MGLMapboxEventAttributes *)attributeDictionary {
    [attributes addEntriesFromDictionary:attributeDictionary];

    return [attributes copy];
}

// Called implicitly from public use of +flush.
//
- (void)postEvents:(NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events {
    if (self.paused) {
        return;
    }

    __weak __typeof__(self) weakSelf = self;
    dispatch_async(self.serialQueue, ^{
        __strong __typeof__(weakSelf) strongSelf = weakSelf;
        [self.apiClient postEvents:events completionHandler:^(NSError * _Nullable error) {
            if (error) {
                [strongSelf pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription: @"Network error",
                                                                                        @"error": error}];
            } else {
                [strongSelf pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription: @"post",
                                                                                   @"debug.eventsCount": @(events.count)}];
            }
            [[UIApplication sharedApplication] endBackgroundTask:_backgroundTaskIdentifier];
            _backgroundTaskIdentifier = UIBackgroundTaskInvalid;
        }];
    });
}

- (void)startTimer {
    [self.timer invalidate];
    self.timer = [NSTimer scheduledTimerWithTimeInterval:MGLFlushInterval
                                                  target:self
                                                selector:@selector(flush)
                                                userInfo:nil
                                                 repeats:YES];
}

- (NSString *)deviceOrientation {
    NSString *result;

    switch ([UIDevice currentDevice].orientation) {
        case UIDeviceOrientationUnknown:
            result = @"Unknown";
            break;
        case UIDeviceOrientationPortrait:
            result = @"Portrait";
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            result = @"PortraitUpsideDown";
            break;
        case UIDeviceOrientationLandscapeLeft:
            result = @"LandscapeLeft";
            break;
        case UIDeviceOrientationLandscapeRight:
            result = @"LandscapeRight";
            break;
        case UIDeviceOrientationFaceUp:
            result = @"FaceUp";
            break;
        case UIDeviceOrientationFaceDown:
            result = @"FaceDown";
            break;
        default:
            result = @"Default - Unknown";
            break;
    }

    return result;
}

- (NSString *)applicationState {
    switch ([UIApplication sharedApplication].applicationState) {
        case UIApplicationStateActive:
            return MGLApplicationStateForeground;
        case UIApplicationStateInactive:
            return MGLApplicationStateInactive;
        case UIApplicationStateBackground:
            return MGLApplicationStateBackground;
        default:
            return MGLApplicationStateUnknown;
    }
}

- (NSInteger)contentSizeScale {
    NSInteger result = -9999;

    NSString *sc = [UIApplication sharedApplication].preferredContentSizeCategory;

    if ([sc isEqualToString:UIContentSizeCategoryExtraSmall]) {
        result = -3;
    } else if ([sc isEqualToString:UIContentSizeCategorySmall]) {
        result = -2;
    } else if ([sc isEqualToString:UIContentSizeCategoryMedium]) {
        result = -1;
    } else if ([sc isEqualToString:UIContentSizeCategoryLarge]) {
        result = 0;
    } else if ([sc isEqualToString:UIContentSizeCategoryExtraLarge]) {
        result = 1;
    } else if ([sc isEqualToString:UIContentSizeCategoryExtraExtraLarge]) {
        result = 2;
    } else if ([sc isEqualToString:UIContentSizeCategoryExtraExtraExtraLarge]) {
        result = 3;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityMedium]) {
        result = -11;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityLarge]) {
        result = 10;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityExtraLarge]) {
        result = 11;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityExtraExtraLarge]) {
        result = 12;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityExtraExtraExtraLarge]) {
        result = 13;
    }

    return result;
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
             @"\n\nSee https://www.mapbox.com/ios-sdk/#telemetry_opt_out for more information."
             @"\n\nAdditionally, by hiding this attribution control you agree to display the required attribution elsewhere in this app."];
        }
    }
}

#pragma mark CLLocationManagerUtilityDelegate

- (void)locationManager:(MGLLocationManager *)locationManager didUpdateLocations:(NSArray *)locations {
    for (CLLocation *loc in locations) {
        double accuracy = 10000000;
        double lat = floor(loc.coordinate.latitude * accuracy) / accuracy;
        double lng = floor(loc.coordinate.longitude * accuracy) / accuracy;
        double horizontalAccuracy = round(loc.horizontalAccuracy);
        NSString *formattedDate = [self.rfc3339DateFormatter stringFromDate:loc.timestamp];
        [MGLMapboxEvents pushEvent:MGLEventTypeLocation withAttributes:@{MGLEventKeyCreated: formattedDate,
                                                                         MGLEventKeyLatitude: @(lat),
                                                                         MGLEventKeyLongitude: @(lng),
                                                                         MGLEventKeyAltitude: @(round(loc.altitude)),
                                                                         MGLEventHorizontalAccuracy: @(horizontalAccuracy)}];
    }
}

- (void)locationManagerBackgroundLocationUpdatesDidAutomaticallyPause:(MGLLocationManager *)locationManager {
    [self pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription:@"locationManager.locationManagerAutoPause"}];
}

- (void)locationManagerBackgroundLocationUpdatesDidTimeout:(MGLLocationManager *)locationManager {
    [self pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription:@"locationManager.locationManagerTimeout"}];
}

- (void)locationManagerDidStartLocationUpdates:(MGLLocationManager *)locationManager {
    [self pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription:@"locationManager.locationManagerStartUpdates"}];
}

- (void)locationManagerDidStopLocationUpdates:(MGLLocationManager *)locationManager {
    [self pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription: @"locationManager.locationManagerStopUpdates"}];
}

#pragma mark MGLMapboxEvents Debug

- (void)pushDebugEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    if (![self debugLoggingEnabled]) {
        return;
    }

    if (!event) {
        return;
    }

    MGLMutableMapboxEventAttributes *evt = [MGLMutableMapboxEventAttributes dictionaryWithDictionary:attributeDictionary];
    [evt setObject:event forKey:@"event"];
    [evt setObject:[self.rfc3339DateFormatter stringFromDate:[NSDate date]] forKey:@"created"];
    [evt setValue:[self applicationState] forKey:@"applicationState"];
    [evt setValue:@([[self class] isEnabled]) forKey:@"telemetryEnabled"];
    [evt setObject:self.instanceID forKey:@"instance"];

    MGLMapboxEventAttributes *finalEvent = [NSDictionary dictionaryWithDictionary:evt];
    [self writeEventToLocalDebugLog:finalEvent];
}

- (void)writeEventToLocalDebugLog:(MGLMapboxEventAttributes *)event {
    if (![self debugLoggingEnabled]) {
        return;
    }

    NSLog(@"%@", [self stringForDebugEvent:event]);

    if (!self.dateForDebugLogFile) {
        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy'-'MM'-'dd"];
        [dateFormatter setTimeZone:[NSTimeZone systemTimeZone]];
        self.dateForDebugLogFile = [dateFormatter stringFromDate:[NSDate date]];
    }

    if (!self.debugLogSerialQueue) {
        NSString *uniqueID = [[NSProcessInfo processInfo] globallyUniqueString];
        self.debugLogSerialQueue = dispatch_queue_create([[NSString stringWithFormat:@"%@.%@.events.debugLog", _appBundleId, uniqueID] UTF8String], DISPATCH_QUEUE_SERIAL);
    }

    dispatch_async(self.debugLogSerialQueue, ^{
        if ([NSJSONSerialization isValidJSONObject:event]) {
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:event options:NSJSONWritingPrettyPrinted error:nil];

            NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            jsonString = [jsonString stringByAppendingString:@",\n"];

            NSString *logFilePath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:[NSString stringWithFormat:@"telemetry_log-%@.json", self.dateForDebugLogFile]];

            NSFileManager *fileManager = [[NSFileManager alloc] init];
            if ([fileManager fileExistsAtPath:logFilePath]) {
                NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:logFilePath];
                [fileHandle seekToEndOfFile];
                [fileHandle writeData:[jsonString dataUsingEncoding:NSUTF8StringEncoding]];
            } else {
                [fileManager createFileAtPath:logFilePath contents:[jsonString dataUsingEncoding:NSUTF8StringEncoding] attributes:@{ NSFileProtectionKey: NSFileProtectionCompleteUntilFirstUserAuthentication }];
            }
        }
    });
}

- (NSString *)stringForDebugEvent:(MGLMapboxEventAttributes *)event {
    // redact potentially sensitive location details from system console log
    if ([event[@"event"] isEqualToString:MGLEventTypeLocation]) {
        MGLMutableMapboxEventAttributes *evt = [MGLMutableMapboxEventAttributes dictionaryWithDictionary:event];
        [evt setObject:@"<redacted>" forKey:@"lat"];
        [evt setObject:@"<redacted>" forKey:@"lng"];
        event = evt;
    }

    return [NSString stringWithFormat:@"Mapbox Telemetry event %@", event];
}

- (BOOL)isProbablyAppStoreBuild {
#if TARGET_IPHONE_SIMULATOR
    return NO;
#else
    // BugshotKit by Marco Arment https://github.com/marcoarment/BugshotKit/
    // Adapted from https://github.com/blindsightcorp/BSMobileProvision

    NSString *binaryMobileProvision = [NSString stringWithContentsOfFile:[NSBundle.mainBundle pathForResource:@"embedded" ofType:@"mobileprovision"] encoding:NSISOLatin1StringEncoding error:NULL];
    if (!binaryMobileProvision) {
        return YES; // no provision
    }

    NSScanner *scanner = [NSScanner scannerWithString:binaryMobileProvision];
    NSString *plistString;
    if (![scanner scanUpToString:@"<plist" intoString:nil] || ! [scanner scanUpToString:@"</plist>" intoString:&plistString]) {
        return YES; // no XML plist found in provision
    }
    plistString = [plistString stringByAppendingString:@"</plist>"];

    NSData *plistdata_latin1 = [plistString dataUsingEncoding:NSISOLatin1StringEncoding];
    NSError *error = nil;
    NSDictionary *mobileProvision = [NSPropertyListSerialization propertyListWithData:plistdata_latin1 options:NSPropertyListImmutable format:NULL error:&error];
    if (error) {
        return YES; // unknown plist format
    }

    if (!mobileProvision || ! mobileProvision.count) {
        return YES; // no entitlements
    }

    if (mobileProvision[@"ProvisionsAllDevices"]) {
        return NO; // enterprise provisioning
    }

    if (mobileProvision[@"ProvisionedDevices"] && [mobileProvision[@"ProvisionedDevices"] count]) {
        return NO; // development or ad-hoc
    }

    return YES; // expected development/enterprise/ad-hoc entitlements not found
#endif
}

@end
