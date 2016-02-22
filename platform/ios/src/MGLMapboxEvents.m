#import "MGLMapboxEvents.h"
#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import "MGLAccountManager.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSBundle+MGLAdditions.h"
#import "NSException+MGLAdditions.h"

#include <mbgl/platform/darwin/reachability.h>
#include <sys/sysctl.h>

static const NSUInteger version = 1;
static NSString *const MGLMapboxEventsUserAgent = @"MapboxEventsiOS/1.1";
static NSString *MGLMapboxEventsAPIBase = @"https://api.tiles.mapbox.com";

NSString *const MGLEventTypeAppUserTurnstile = @"appUserTurnstile";
NSString *const MGLEventTypeMapLoad = @"map.load";
NSString *const MGLEventTypeMapTap = @"map.click";
NSString *const MGLEventTypeMapDragEnd = @"map.dragend";
NSString *const MGLEventTypeLocation = @"location";
NSString *const MGLEventTypeVisit = @"visit";
NSString *const MGLEventTypeLocalDebug = @"debug";

NSString *const MGLEventKeyLatitude = @"lat";
NSString *const MGLEventKeyLongitude = @"lng";
NSString *const MGLEventKeyZoomLevel = @"zoom";
NSString *const MGLEventKeySpeed = @"speed";
NSString *const MGLEventKeyCourse = @"course";
NSString *const MGLEventKeyAltitude = @"altitude";
NSString *const MGLEventKeyHorizontalAccuracy = @"horizontalAccuracy";
NSString *const MGLEventKeyVerticalAccuracy = @"verticalAccuracy";
NSString *const MGLEventKeyPushEnabled = @"enabled.push";
NSString *const MGLEventKeyEmailEnabled = @"enabled.email";
NSString *const MGLEventKeyGestureID = @"gesture";
NSString *const MGLEventKeyArrivalDate = @"arrivalDate";
NSString *const MGLEventKeyDepartureDate = @"departureDate";
NSString *const MGLEventKeyLocalDebugDescription = @"debug.description";

NSString *const MGLEventGestureSingleTap = @"SingleTap";
NSString *const MGLEventGestureDoubleTap = @"DoubleTap";
NSString *const MGLEventGestureTwoFingerSingleTap = @"TwoFingerTap";
NSString *const MGLEventGestureQuickZoom = @"QuickZoom";
NSString *const MGLEventGesturePanStart = @"Pan";
NSString *const MGLEventGesturePinchStart = @"Pinch";
NSString *const MGLEventGestureRotateStart = @"Rotation";
NSString *const MGLEventGesturePitchStart = @"Pitch";

const NSUInteger MGLMaximumEventsPerFlush = 20;
const NSTimeInterval MGLFlushInterval = 60;

@interface MGLMapboxEventsData : NSObject

@property (nonatomic) NSString *vendorId;
@property (nonatomic) NSString *model;
@property (nonatomic) NSString *iOSVersion;
@property (nonatomic) NSString *carrier;
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

@interface MGLMapboxEvents () <CLLocationManagerDelegate>

@property (nonatomic) MGLMapboxEventsData *data;
@property (nonatomic, copy) NSString *appBundleId;
@property (nonatomic, copy) NSString *appName;
@property (nonatomic, copy) NSString *appVersion;
@property (nonatomic, copy) NSString *appBuildNumber;
@property (nonatomic, copy) NSString *instanceID;
@property (nonatomic, copy) NSString *dateForDebugLogFile;
@property (nonatomic, copy) NSData *digicertCert;
@property (nonatomic, copy) NSData *geoTrustCert;
@property (nonatomic, copy) NSData *testServerCert;
@property (nonatomic) NSDateFormatter *rfc3339DateFormatter;
@property (nonatomic) NSURLSession *session;
@property (nonatomic) BOOL usesTestServer;
@property (nonatomic) BOOL canEnableDebugLogging;
@property (nonatomic, getter=isPaused) BOOL paused;
@property (nonatomic) NS_MUTABLE_ARRAY_OF(MGLMapboxEventAttributes *) *eventQueue;
@property (nonatomic) dispatch_queue_t serialQueue;
@property (nonatomic) dispatch_queue_t debugLogSerialQueue;
@property (nonatomic) CLLocationManager *locationManager;
@property (nonatomic) NSTimer *timer;

@end

@implementation MGLMapboxEvents {
    id _userDefaultsObserver;
}

+ (void)initialize {
    if (self == [MGLMapboxEvents class]) {
        NSBundle *bundle = [NSBundle mainBundle];
        NSNumber *accountTypeNumber = [bundle objectForInfoDictionaryKey:@"MGLMapboxAccountType"];
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{
             @"MGLMapboxAccountType": accountTypeNumber ? accountTypeNumber : @0,
             @"MGLMapboxMetricsEnabled": @YES,
             @"MGLMapboxMetricsDebugLoggingEnabled": @NO,
         }];
    }
}

+ (BOOL)isEnabled {
    return ([[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"] &&
            [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"] == 0);
}

- (BOOL)debugLoggingEnabled {
    return (self.canEnableDebugLogging &&
            [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"]);
}

- (instancetype) init {
    self = [super init];
    if (self) {
        _appBundleId = [[NSBundle mainBundle] bundleIdentifier];
        _appName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"];
        _appVersion = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
        _appBuildNumber = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];
        _instanceID = [[NSUUID UUID] UUIDString];

        NSString *uniqueID = [[NSProcessInfo processInfo] globallyUniqueString];
        _serialQueue = dispatch_queue_create([[NSString stringWithFormat:@"%@.%@.events.serial", _appBundleId, uniqueID] UTF8String], DISPATCH_QUEUE_SERIAL);

        // Configure Events Infrastructure
        // ===============================

        // Check for TEST Metrics URL
        NSString *testURL = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLMetricsTestServerURL"];
        if (testURL != nil) {
            MGLMapboxEventsAPIBase = testURL;
            _usesTestServer = YES;
        } else {
            // Explicitly Set For Clarity
            _usesTestServer = NO;
        }

        _paused = YES;
        [self resumeMetricsCollection];
        NSBundle *resourceBundle = [NSBundle mgl_frameworkBundle];

        // Load Local Copy of Server's Public Key
        NSString *cerPath = nil;
        cerPath = [resourceBundle pathForResource:@"api_mapbox_com-geotrust" ofType:@"der"];
        if (cerPath != nil) {
            _geoTrustCert = [NSData dataWithContentsOfFile:cerPath];
        }

        cerPath = [resourceBundle pathForResource:@"api_mapbox_com-digicert" ofType:@"der"];
        if (cerPath != nil) {
            _digicertCert = [NSData dataWithContentsOfFile:cerPath];
        }
        cerPath = [resourceBundle pathForResource:@"star_tilestream_net" ofType:@"der"];
        if (cerPath != nil) {
            _testServerCert = [NSData dataWithContentsOfFile:cerPath];
        }

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
        
        // Enable Battery Monitoring
        [UIDevice currentDevice].batteryMonitoringEnabled = YES;

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
        __weak MGLMapboxEvents *weakSelf = self;
        _userDefaultsObserver = [[NSNotificationCenter defaultCenter] addObserverForName:NSUserDefaultsDidChangeNotification
                                                                                  object:nil
                                                                                   queue:[NSOperationQueue mainQueue]
                                                                              usingBlock:
         ^(NSNotification *notification) {
             MGLMapboxEvents *strongSelf = weakSelf;
             [strongSelf validate];
         }];
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

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:_userDefaultsObserver];
    [self pauseMetricsCollection];
}

+ (void)validate {
    [[MGLMapboxEvents sharedManager] validate];
}

- (void)validate {
    BOOL enabledInSettings = [[self class] isEnabled];
    
    if (self.paused && enabledInSettings) {
        [self resumeMetricsCollection];
    } else if (!self.paused && !enabledInSettings) {
        [self pauseMetricsCollection];
    }
   
    [self validateUpdatingLocation];
}

- (void)validateUpdatingLocation {
    if (self.paused) {
        [self stopUpdatingLocation];
    } else {
        switch ([CLLocationManager authorizationStatus]) {
            case kCLAuthorizationStatusNotDetermined:
            case kCLAuthorizationStatusRestricted:
            case kCLAuthorizationStatusDenied:
                [self stopUpdatingLocation];
                break;
            case kCLAuthorizationStatusAuthorized:
                // Also handles kCLAuthorizationStatusAuthorizedAlways
                [self startUpdatingLocation];
                break;
            case kCLAuthorizationStatusAuthorizedWhenInUse:
                if (UIApplication.sharedApplication.applicationState == UIApplicationStateBackground) {
                    // Prevent blue status bar when app is not in foreground
                    [self stopUpdatingLocation];
                } else {
                    [self startUpdatingLocation];
                }
                break;
        }
    }
}

+ (void)pauseMetricsCollection {
    [[MGLMapboxEvents sharedManager] pauseMetricsCollection];
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
    [self.session invalidateAndCancel];
    self.session = nil;
    
    [self validateUpdatingLocation];
}

- (void)stopUpdatingLocation {
    [self.locationManager stopUpdatingLocation];
    
    // -[CLLocationManager stopMonitoringVisits] is only available in iOS 8+.
    if ([self.locationManager respondsToSelector:@selector(stopMonitoringVisits)]) {
        [self.locationManager stopMonitoringVisits];
    }
    
    self.locationManager = nil;
}

+ (void)resumeMetricsCollection {
    [[MGLMapboxEvents sharedManager] resumeMetricsCollection];
}

- (void)resumeMetricsCollection {
    if (!self.paused || ![[self class] isEnabled]) {
        return;
    }
    
    self.paused = NO;
    self.data = [[MGLMapboxEventsData alloc] init];
    self.session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:nil];
    
    [self validateUpdatingLocation];
}

- (void)startUpdatingLocation {
    if (self.locationManager || self.paused) {
        NSAssert(!(self.locationManager && self.paused),
                 @"MGLMapboxEvents should not have a CLLocationManager while paused.");
        return;
    }
    
    CLLocationManager *locationManager = [[CLLocationManager alloc] init];
    locationManager.delegate = self;
    locationManager.desiredAccuracy = kCLLocationAccuracyKilometer;
    locationManager.distanceFilter = 10;
    [locationManager startUpdatingLocation];

    // -[CLLocationManager startMonitoringVisits] is only available in iOS 8+.
    if ([locationManager respondsToSelector:@selector(startMonitoringVisits)]) {
        [locationManager startMonitoringVisits];
    }
    
    self.locationManager = locationManager;
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
    
    if ([self isPaused]) {
        return;
    }
    
    NSDictionary *stockAttributes = @{@"event": event,
                                      @"version": @(version),
                                      @"created": [self.rfc3339DateFormatter stringFromDate:[NSDate date]],
                                      @"instance": self.instanceID,
                                      @"vendorId": self.data.vendorId,
                                      @"appBundleId": self.appBundleId,
                                      @"model": self.data.model,
                                      @"operatingSystem": self.data.iOSVersion,
                                      @"resolution": @(self.data.scale),
                                      @"wifi": @([[MGLReachability reachabilityForLocalWiFi] isReachableViaWiFi]),
                                      @"orientation": [self deviceOrientation],
                                      @"batteryLevel": @([self batteryLevel]),
                                      @"applicationState": [self applicationState],
                                      @"accessibilityFontScale": @([self contentSizeScale])};
    MGLMutableMapboxEventAttributes *allEventAttributes = [NSMutableDictionary dictionaryWithDictionary:attributeDictionary];
    [allEventAttributes addEntriesFromDictionary:stockAttributes];
    MGLMapboxEventAttributes *finalEvent = [allEventAttributes copy];
    [self.eventQueue addObject:finalEvent];
    [self writeEventToLocalDebugLog:finalEvent];
    
    // Has Flush Limit Been Reached?
    if (self.eventQueue.count >= MGLMaximumEventsPerFlush) {
        [self flush];
    } else if (self.eventQueue.count ==  1) {
        // If this is first new event on queue start timer,
        [self startTimer];
    }
}

- (void)pushTurnstileEvent {
    __weak MGLMapboxEvents *weakSelf = self;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        MGLMapboxEvents *strongSelf = weakSelf;
        
        if (!strongSelf) {
            return;
        }
        
        NSString *vendorID = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        if (!vendorID) {
            return;
        }
        
        NSDictionary *vevt = @{@"event" : MGLEventTypeAppUserTurnstile,
                               @"created" : [strongSelf.rfc3339DateFormatter stringFromDate:[NSDate date]],
                               @"appBundleId" : strongSelf.appBundleId,
                               @"vendorId": vendorID,
                               @"version": @(version),
                               @"instance": strongSelf.instanceID};
        
        [strongSelf.eventQueue addObject:vevt];
        [strongSelf flush];
        [strongSelf writeEventToLocalDebugLog:vevt];
    });
}

+ (void)flush {
    [[MGLMapboxEvents sharedManager] flush];
}

- (void)flush {
    if ([MGLAccountManager accessToken] == nil) {
        return;
    }
    
    if ([self.eventQueue count] == 0) {
        return;
    }
    
    NSArray *events = [NSArray arrayWithArray:self.eventQueue];
    [self.eventQueue removeAllObjects];
    
    [self postEvents:events];
    
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }

    [MGLMapboxEvents pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription:@"flush"}];
}

// Called implicitly from public use of +flush.
//
- (void)postEvents:(NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events {
    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(self.serialQueue, ^{
        MGLMapboxEvents *strongSelf = weakSelf;
        
        if (!strongSelf) {
            return;
        }

        NSString *url = [NSString stringWithFormat:@"%@/events/v1?access_token=%@", MGLMapboxEventsAPIBase, [MGLAccountManager accessToken]];
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
        [request setValue:strongSelf.userAgent forHTTPHeaderField:@"User-Agent"];
        [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPMethod:@"POST"];
        
        if ([NSJSONSerialization isValidJSONObject:events]) {
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:events options:NSJSONWritingPrettyPrinted error:nil];
            [request setHTTPBody:jsonData];

            if (!strongSelf.paused) {
                [[strongSelf.session dataTaskWithRequest:request] resume];
            } else {
                for (MGLMapboxEventAttributes *event in events) {
                    if ([event[@"event"] isEqualToString:MGLEventTypeAppUserTurnstile]) {
                        NSURLSession *temporarySession = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
                                                                                       delegate:strongSelf
                                                                                  delegateQueue:nil];
                        [[temporarySession dataTaskWithRequest:request] resume];
                        [temporarySession finishTasksAndInvalidate];
                    }
                }
            }

            [MGLMapboxEvents pushDebugEvent:MGLEventTypeLocalDebug withAttributes:@{MGLEventKeyLocalDebugDescription: @"post",
                                                                                    @"debug.eventsCount": @(events.count)}];
        }
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

- (NSString *)userAgent {
    return [NSString stringWithFormat:@"%@/%@/%@ %@", self.appName, self.appVersion, self.appBuildNumber, MGLMapboxEventsUserAgent];
}

- (NSInteger)batteryLevel {
    return [[NSNumber numberWithFloat:100 * [UIDevice currentDevice].batteryLevel] integerValue];
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
    NSString *result;

    switch ([UIApplication sharedApplication].applicationState) {
        case UIApplicationStateActive:
            result = @"Active";
            break;
        case UIApplicationStateInactive:
            result = @"Inactive";
            break;
        case UIApplicationStateBackground:
            result = @"Background";
            break;
        default:
            result = @"Default - Unknown";
            break;
    }
    
    return result;
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

+ (BOOL)checkPushEnabled {
    BOOL blockResult;
    if ([[UIApplication sharedApplication] respondsToSelector:@selector(isRegisteredForRemoteNotifications)]) {
        // iOS 8+
        blockResult = [[UIApplication sharedApplication] isRegisteredForRemoteNotifications];
    } else {
        // iOS 7
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        UIRemoteNotificationType types = [[UIApplication sharedApplication] enabledRemoteNotificationTypes];
        blockResult = (types == UIRemoteNotificationTypeNone) ? NO : YES;
#pragma clang diagnostic pop
    }
    return blockResult;
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

#pragma mark CLLocationManagerDelegate

- (void)locationManager:(CLLocationManager *)manager didUpdateLocations:(NSArray *)locations {
    //  Iterate through locations to pass all data
    for (CLLocation *loc in locations) {
        [MGLMapboxEvents pushEvent:MGLEventTypeLocation withAttributes:@{
            MGLEventKeyLatitude: @(loc.coordinate.latitude),
            MGLEventKeyLongitude: @(loc.coordinate.longitude),
            MGLEventKeySpeed: @(loc.speed),
            MGLEventKeyCourse: @(loc.course),
            MGLEventKeyAltitude: @(round(loc.altitude)),
            MGLEventKeyHorizontalAccuracy: @(round(loc.horizontalAccuracy)),
            MGLEventKeyVerticalAccuracy: @(round(loc.verticalAccuracy))
        }];
    }
}

- (void)locationManager:(CLLocationManager *)manager didVisit:(CLVisit *)visit {
    [MGLMapboxEvents pushEvent:MGLEventTypeVisit withAttributes:@{
        MGLEventKeyLatitude: @(visit.coordinate.latitude),
        MGLEventKeyLongitude: @(visit.coordinate.longitude),
        MGLEventKeyHorizontalAccuracy: @(round(visit.horizontalAccuracy)),
        MGLEventKeyArrivalDate: [[NSDate distantPast] isEqualToDate:visit.arrivalDate] ? [NSNull null] : [_rfc3339DateFormatter stringFromDate:visit.arrivalDate],
        MGLEventKeyDepartureDate: [[NSDate distantFuture] isEqualToDate:visit.departureDate] ? [NSNull null] : [_rfc3339DateFormatter stringFromDate:visit.departureDate]
    }];
}

- (void)locationManager:(CLLocationManager *)manager didChangeAuthorizationStatus:(CLAuthorizationStatus)status {
    [self validateUpdatingLocation];
}

#pragma mark NSURLSessionDelegate

- (void)URLSession:(NSURLSession *)session didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^) (NSURLSessionAuthChallengeDisposition disposition, NSURLCredential *credential))completionHandler {

    if([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust]) {

        SecTrustRef serverTrust = [[challenge protectionSpace] serverTrust];
        SecTrustResultType trustResult;

        // Validate the certificate chain with the device's trust store anyway
        // This *might* give use revocation checking
        SecTrustEvaluate(serverTrust, &trustResult);
        if (trustResult == kSecTrustResultUnspecified)
        {
            // Look for a pinned certificate in the server's certificate chain
            long numKeys = SecTrustGetCertificateCount(serverTrust);

            BOOL found = NO;
            // Try GeoTrust Cert First
            for (int lc = 0; lc < numKeys; lc++) {
                SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
                NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));

                // Compare Remote Key With Local Version
                if ([remoteCertificateData isEqualToData:_geoTrustCert]) {
                    // Found the certificate; continue connecting
                    completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                    found = YES;
                    break;
                }
            }

            if (!found) {
                // Fallback to Digicert Cert
                for (int lc = 0; lc < numKeys; lc++) {
                    SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
                    NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));

                    // Compare Remote Key With Local Version
                    if ([remoteCertificateData isEqualToData:_digicertCert]) {
                        // Found the certificate; continue connecting
                        completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                        found = YES;
                        break;
                    }
                }

                if (!found && _usesTestServer) {
                    // See if this is test server
                    for (int lc = 0; lc < numKeys; lc++) {
                        SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
                        NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));

                        // Compare Remote Key With Local Version
                        if ([remoteCertificateData isEqualToData:_testServerCert]) {
                            // Found the certificate; continue connecting
                            completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                            found = YES;
                            break;
                        }
                    }
                }

                if (!found) {
                    // The certificate wasn't found in GeoTrust nor Digicert. Cancel the connection.
                    completionHandler(NSURLSessionAuthChallengeCancelAuthenticationChallenge, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                }
            }
        }
        else
        {
            // Certificate chain validation failed; cancel the connection
            completionHandler(NSURLSessionAuthChallengeCancelAuthenticationChallenge, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
        }
    }

}

#pragma mark MGLMapboxEvents Debug

+ (void)pushDebugEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    [[MGLMapboxEvents sharedManager] pushDebugEvent:event withAttributes:attributeDictionary];
}

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
    if ([event[@"event"] isEqualToString:MGLEventTypeLocation] ||
        [event[@"event"] isEqualToString:MGLEventTypeVisit]) {
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
