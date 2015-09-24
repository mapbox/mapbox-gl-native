#import "MGLMapboxEvents.h"

#import <UIKit/UIKit.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>
#import <CoreLocation/CoreLocation.h>

#include <mbgl/platform/darwin/reachability.h>

#import "MGLAccountManager.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSBundle+MGLAdditions.h"
#import "NSException+MGLAdditions.h"

#include <sys/sysctl.h>

static const NSUInteger version = 1;
static NSString *const MGLMapboxEventsUserAgent = @"MapboxEventsiOS/1.0";
static NSString *MGLMapboxEventsAPIBase = @"https://api.tiles.mapbox.com";

NSString *const MGLEventTypeAppUserTurnstile = @"appUserTurnstile";
NSString *const MGLEventTypeMapLoad = @"map.load";
NSString *const MGLEventTypeMapTap = @"map.click";
NSString *const MGLEventTypeMapDragEnd = @"map.dragend";
NSString *const MGLEventTypeLocation = @"location";
NSString *const MGLEventTypeVisit = @"visit";

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

// All of the following properties are written to only from
// the main thread, but can be read on any thread.
//
@property (atomic) NSString *vendorId;
@property (atomic) NSString *model;
@property (atomic) NSString *iOSVersion;
@property (atomic) NSString *carrier;
@property (atomic) CGFloat scale;

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
        CTCarrier *carrierVendor = [[[CTTelephonyNetworkInfo alloc] init] subscriberCellularProvider];
        _carrier = [carrierVendor carrierName];
    }
    return self;
}

// Can be called from any thread.
//
- (NSString *)sysInfoByName:(char *)typeSpecifier
{
    size_t size;
    sysctlbyname(typeSpecifier, NULL, &size, NULL, 0);
    
    char *answer = malloc(size);
    sysctlbyname(typeSpecifier, answer, &size, NULL, 0);
    
    NSString *results = [NSString stringWithCString:answer encoding: NSUTF8StringEncoding];
    
    free(answer);
    return results;
}

@end

//
// Threadsafety conventions:
//
// All variables accessed from more than one thread are
// designated `atomic` and accessed through dot syntax. The
// main thread uses underscore syntax during the
// initialization of the variable.
//
// All variables accessed outside of initialization and
// from within a single thread use underscore syntax.
//
// All captures of `self` from within asynchronous
// dispatches will use a `strongSelf` to avoid cyclical
// strong references.
//

@interface MGLMapboxEvents () <CLLocationManagerDelegate>

// All of the following properties are written to only from
// the main thread, but can be read on any thread.
//
@property (atomic) MGLMapboxEventsData *data;
@property (atomic) NSString *appBundleId;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;
@property (atomic) NSString *appBuildNumber;
@property (atomic) NSString *instanceID;
@property (atomic) NSDateFormatter *rfc3339DateFormatter;
@property (atomic) NSURLSession *session;
@property (atomic) NSData *digicertCert;
@property (atomic) NSData *geoTrustCert;
@property (atomic) NSData *testServerCert;
@property (atomic) BOOL usesTestServer;

// Main thread only
@property (nonatomic) CLLocationManager *locationManager;

// The paused state tracker is only ever accessed from the main thread.
//
@property (nonatomic, getter=isPaused) BOOL paused;

// The timer is only ever accessed from the main thread.
//
@property (nonatomic) NSTimer *timer;

// This is an array of events to push. All access to it will be
// from our own serial queue.
//
@property (nonatomic) NS_MUTABLE_ARRAY_OF(MGLMapboxEventAttributes *) *eventQueue;

// This is a custom serial queue for accessing the event queue.
//
@property (nonatomic) dispatch_queue_t serialQueue;

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
         }];
    }
}

+ (BOOL)isEnabled {
    return ([[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"] &&
            [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"] == 0);
}

// Must be called from the main thread. Only called internally.
//
- (instancetype) init {
    MGLAssertIsMainThread();

    self = [super init];
    if (self) {
        if (! [MGLAccountManager mapboxMetricsEnabledSettingShownInApp] &&
            [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"] == 0) {
            // Opt Out is not configured in UI, so check for Settings.bundle
            // Put Settings bundle into memory
            id defaultEnabledValue;
            NSString *appSettingsBundle = [[NSBundle mainBundle] pathForResource:@"Settings" ofType:@"bundle"];

            if (appSettingsBundle) {
                // Dynamic Settings.bundle loading based on:
                // http://stackoverflow.com/questions/510216/can-you-make-the-settings-in-settings-bundle-default-even-if-you-dont-open-the
                NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[appSettingsBundle stringByAppendingPathComponent:@"Root.plist"]];
                NSArray *preferences = settings[@"PreferenceSpecifiers"];
                for (NSDictionary *prefSpecification in preferences) {
                    if ([prefSpecification[@"Key"] isEqualToString:@"MGLMapboxMetricsEnabled"]) {
                        defaultEnabledValue = prefSpecification[@"DefaultValue"];
                    }
                }
            }

            if (!defaultEnabledValue)
            {
                [NSException raise:@"MGLMapboxMetricsEnabled setting missing" format:
                 @"End users must be able to opt out of Metrics in your app, either inside Settings (via Settings.bundle) or inside this app. "
                 @"If you implement the opt-out control inside this app, disable this assertion by setting MGLMapboxMetricsEnabledSettingShownInApp to YES in Info.plist."];
            }
            [[NSUserDefaults standardUserDefaults] registerDefaults:@{
                 @"MGLMapboxMetricsEnabled": defaultEnabledValue,
             }];
        }

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
        NSBundle *resourceBundle = [NSBundle bundleWithPath:[NSBundle mgl_resourceBundlePath]];

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
        
        __weak MGLMapboxEvents *weakSelf = self;
        _userDefaultsObserver = [[NSNotificationCenter defaultCenter] addObserverForName:NSUserDefaultsDidChangeNotification
                                                                                  object:nil
                                                                                   queue:[NSOperationQueue mainQueue]
                                                                              usingBlock:
         ^(NSNotification *notification) {
             MGLMapboxEvents *strongSelf = weakSelf;
             [strongSelf validate];
         }];

        // Turn the Mapbox Turnstile to Count App Users
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
            [self pushTurnstileEvent];
        });
    }
    return self;
}

// Can be called from any thread. Called implicitly from any
// public class convenience methods. May return nil if this feature is disabled.
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

// Used to determine if Mapbox Metrics should be collected at any given point in time
- (void)validate {
    MGLAssertIsMainThread();
    BOOL enabledInSettings = [[self class] isEnabled];
    if (self.paused && enabledInSettings) {
        [self resumeMetricsCollection];
    } else if (!self.paused && !enabledInSettings) {
        [self pauseMetricsCollection];
    }
    
    [self validateUpdatingLocation];
}

- (void)validateUpdatingLocation {
    MGLAssertIsMainThread();
    if (self.paused) {
        [self stopUpdatingLocation];
    } else {
        CLAuthorizationStatus authStatus = [CLLocationManager authorizationStatus];
        if (authStatus == kCLAuthorizationStatusDenied ||
            authStatus == kCLAuthorizationStatusRestricted) {
            [self stopUpdatingLocation];
        } else if (authStatus == kCLAuthorizationStatusAuthorized ||
                   authStatus == kCLAuthorizationStatusAuthorizedWhenInUse) {
            [self startUpdatingLocation];
        }
    }
}

+ (void)pauseMetricsCollection {
    [[MGLMapboxEvents sharedManager] pauseMetricsCollection];
}

// Must be called from the main thread.
//
- (void)pauseMetricsCollection {
    MGLAssertIsMainThread();
    if (self.paused) {
        return;
    }
    self.paused = YES;
    [_timer invalidate];
    _timer = nil;
    [_eventQueue removeAllObjects];
    _data = nil;
    [_session invalidateAndCancel];
    _session = nil;
    
    [self validateUpdatingLocation];
}

- (void)stopUpdatingLocation {
    [_locationManager stopUpdatingLocation];
    
    // -[CLLocationManager stopMonitoringVisits] is only available in iOS 8+.
    if ([_locationManager respondsToSelector:@selector(stopMonitoringVisits)]) {
        [_locationManager stopMonitoringVisits];
    }
    
    _locationManager = nil;
}

+ (void)resumeMetricsCollection {
    [[MGLMapboxEvents sharedManager] resumeMetricsCollection];
}

// Must be called from the main thread.
//
- (void)resumeMetricsCollection {
    MGLAssertIsMainThread();
    if (!self.paused || ![[self class] isEnabled]) {
        return;
    }
    self.paused = NO;
    _data = [[MGLMapboxEventsData alloc] init];
    _session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:nil];
    
    [self validateUpdatingLocation];
}

- (void)startUpdatingLocation {
    MGLAssertIsMainThread();
    if (_locationManager || _paused) {
        NSAssert(!(_locationManager && _paused),
                 @"MGLMapboxEvents should not have a CLLocationManager while paused.");
        return;
    }
    _locationManager = [[CLLocationManager alloc] init];
    _locationManager.desiredAccuracy = kCLLocationAccuracyKilometer;
    _locationManager.distanceFilter = 10;
    _locationManager.delegate = self;
    
    [_locationManager startUpdatingLocation];
    
    // -[CLLocationManager startMonitoringVisits] is only available in iOS 8+.
    if ([_locationManager respondsToSelector:@selector(startMonitoringVisits)]) {
        [_locationManager startMonitoringVisits];
    }
}

- (void) pushTurnstileEvent {

    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(_serialQueue, ^{

        MGLMapboxEvents *strongSelf = weakSelf;

        if ( ! strongSelf) return;

            // Build only IDFV event
            NSString *vid = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
            NSDictionary *vevt = @{
                @"event" : MGLEventTypeAppUserTurnstile,
                @"created" : [strongSelf.rfc3339DateFormatter stringFromDate:[NSDate date]],
                @"appBundleId" : strongSelf.appBundleId,
                @"vendorId": vid,
                @"version": @(version),
                @"instance": strongSelf.instanceID
            };

            // Add to Queue
            [_eventQueue addObject:vevt];

            // Flush
            [strongSelf flush];
    });
}

// Can be called from any thread. Can be called rapidly from
// the UI thread, so performance is paramount.
//
+ (void) pushEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        [[MGLMapboxEvents sharedManager] pushEvent:event withAttributes:attributeDictionary];
    });
}

// Can be called from any thread. Called implicitly from public
// use of +pushEvent:withAttributes:.
//
- (void) pushEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(_serialQueue, ^{

        MGLMapboxEvents *strongSelf = weakSelf;

        if ( ! strongSelf) return;

        // Metrics Collection Has Been Paused
        if (_paused) {
            return;
        }
        
        if (!event) return;

        MGLMutableMapboxEventAttributes *evt = [MGLMutableMapboxEventAttributes dictionaryWithDictionary:attributeDictionary];
        // mapbox-events stock attributes
        [evt setObject:event forKey:@"event"];
        [evt setObject:@(version) forKey:@"version"];
        [evt setObject:[strongSelf.rfc3339DateFormatter stringFromDate:[NSDate date]] forKey:@"created"];
        [evt setObject:strongSelf.instanceID forKey:@"instance"];
        [evt setObject:strongSelf.data.vendorId forKey:@"vendorId"];
        [evt setObject:strongSelf.appBundleId forKeyedSubscript:@"appBundleId"];
        
        // mapbox-events-ios stock attributes
        [evt setValue:strongSelf.data.model forKey:@"model"];
        [evt setValue:strongSelf.data.iOSVersion forKey:@"operatingSystem"];
        [evt setValue:[strongSelf deviceOrientation] forKey:@"orientation"];
        [evt setValue:@((int)(100 * [UIDevice currentDevice].batteryLevel)) forKey:@"batteryLevel"];
        [evt setValue:@(strongSelf.data.scale) forKey:@"resolution"];
        [evt setValue:strongSelf.data.carrier forKey:@"carrier"];
        
        NSString *cell = [strongSelf currentCellularNetworkConnectionType];
        if (cell) {
            [evt setValue:cell forKey:@"cellularNetworkType"];
        } else {
            [evt setObject:[NSNull null] forKey:@"cellularNetworkType"];
        }
        
        MGLReachability *reachability = [MGLReachability reachabilityForLocalWiFi];
        [evt setValue:([reachability isReachableViaWiFi] ? @YES : @NO) forKey:@"wifi"];
        
        [evt setValue:@([strongSelf contentSizeScale]) forKey:@"accessibilityFontScale"];

        // Make Immutable Version
        NSDictionary *finalEvent = [NSDictionary dictionaryWithDictionary:evt];
        
        // Put On The Queue
        [_eventQueue addObject:finalEvent];
        
        // Has Flush Limit Been Reached?
        if (_eventQueue.count >= MGLMaximumEventsPerFlush) {
            [strongSelf flush];
        } else if (_eventQueue.count ==  1) {
            // If this is first new event on queue start timer,
            [strongSelf startTimer];
        }
    });
}

// Can be called from any thread.
//
+ (void) flush {
    [[MGLMapboxEvents sharedManager] flush];
}

// Can be called from any thread.
//
- (void) flush {
    if ([MGLAccountManager accessToken] == nil) return;

    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(_serialQueue, ^{
        MGLMapboxEvents *strongSelf = weakSelf;
        if ( ! strongSelf || [_eventQueue count] == 0) return;

        // Make an immutable copy
        NSArray *events = [NSArray arrayWithArray:_eventQueue];

        // Update Queue to remove events sent to server
        [_eventQueue removeAllObjects];

        // Send Array of Events to Server
        [strongSelf postEvents:events];

        // Cancel Any Timer That May Running
        if (strongSelf.timer) {
            [strongSelf.timer invalidate];
            strongSelf.timer = nil;
        }
    });
}

// Can be called from any thread. Called implicitly from public
// use of +flush. Posts an async network request to upload metrics.
//
- (void) postEvents:(NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events {
    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        MGLMapboxEvents *strongSelf = weakSelf;
        if (!strongSelf) return;

        // Setup URL Request
        NSString *url = [NSString stringWithFormat:@"%@/events/v1?access_token=%@", MGLMapboxEventsAPIBase, [MGLAccountManager accessToken]];
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
        [request setValue:strongSelf.userAgent forHTTPHeaderField:@"User-Agent"];
        [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPMethod:@"POST"];
        
        // Convert Array of Dictionaries to JSON
        if ([NSJSONSerialization isValidJSONObject:events]) {
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:events options:NSJSONWritingPrettyPrinted error:nil];
            [request setHTTPBody:jsonData];

            // Send non blocking HTTP Request to server
            [[_session dataTaskWithRequest:request] resume];
        }
    });
}

// Can be called from any thread.
//
- (void) startTimer {
    void (^timerBlock)() = ^{
        // Stop Timer if it already exists
        [_timer invalidate];

        // Start New Timer
        _timer = [NSTimer scheduledTimerWithTimeInterval:MGLFlushInterval
                                                  target:self
                                                selector:@selector(flush)
                                                userInfo:nil
                                                 repeats:YES];
    };

    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            timerBlock();
        });
    } else {
        timerBlock();
    }
}

// Can be called from any thread.
//
- (NSString *) userAgent {
    return [NSString stringWithFormat:@"%@/%@/%@ %@", self.appName, self.appVersion, self.appBuildNumber, MGLMapboxEventsUserAgent];
}

// Can be called from any thread.
//
- (NSString *) deviceOrientation {
    __block NSString *result;

    NSString *(^deviceOrientationBlock)(void) = ^{
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
    };

    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            result = deviceOrientationBlock();
        });
    } else {
        result = deviceOrientationBlock();
    }

    return result;
}

// Can be called from any thread.
//
- (NSInteger) contentSizeScale {
    __block NSInteger result = -9999;

    NSInteger (^contentSizeScaleBlock)(void) = ^{
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
    };

    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            result = contentSizeScaleBlock();
        });
    } else {
        result = contentSizeScaleBlock();
    }

    return result;
}

// Can be called from any thread.
//
- (NSString *) currentCellularNetworkConnectionType {
    CTTelephonyNetworkInfo *telephonyInfo = [[CTTelephonyNetworkInfo alloc] init];
    NSString *radioTech = telephonyInfo.currentRadioAccessTechnology;
    
    if (radioTech == nil) {
        return nil;
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyGPRS]) {
        return @"GPRS";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyEdge]) {
        return @"EDGE";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyWCDMA]) {
        return @"WCDMA";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyHSDPA]) {
        return @"HSDPA";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyHSUPA]) {
        return @"HSUPA";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyCDMA1x]) {
        return @"CDMA1x";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyCDMAEVDORev0]) {
        return @"CDMAEVDORev0";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyCDMAEVDORevA]) {
        return @"CDMAEVDORevA";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyCDMAEVDORevB]) {
        return @"CDMAEVDORevB";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyeHRPD]) {
        return @"HRPD";
    } else if ([radioTech isEqualToString:CTRadioAccessTechnologyLTE]) {
        return @"LTE";
    } else {
        return @"Unknown";
    }
}

// Can be called from any thread.
//
+ (BOOL) checkPushEnabled {
    BOOL (^pushCheckBlock)(void) = ^{
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
    };

    __block BOOL result;

    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            result = pushCheckBlock();
        });
    } else {
        result = pushCheckBlock();
    }

    return result;
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
            MGLEventKeyAltitude: @(loc.altitude),
            MGLEventKeyHorizontalAccuracy: @(loc.horizontalAccuracy),
            MGLEventKeyVerticalAccuracy: @(loc.verticalAccuracy)
        }];
    }
}

- (void)locationManager:(CLLocationManager *)manager didVisit:(CLVisit *)visit {
    [MGLMapboxEvents pushEvent:MGLEventTypeVisit withAttributes:@{
        MGLEventKeyLatitude: @(visit.coordinate.latitude),
        MGLEventKeyLongitude: @(visit.coordinate.longitude),
        MGLEventKeyHorizontalAccuracy: @(visit.horizontalAccuracy),
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

@end
