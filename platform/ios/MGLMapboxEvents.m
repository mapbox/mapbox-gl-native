#import "MGLMapboxEvents.h"

#import <UIKit/UIKit.h>
#import <SystemConfiguration/CaptiveNetwork.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>

#import "MGLMetricsLocationManager.h"
#import "NSProcessInfo+MGLAdditions.h"
#import "NSBundle+MGLAdditions.h"
#import "NSException+MGLAdditions.h"

#include <sys/sysctl.h>

static NSString *const MGLMapboxEventsUserAgent = @"MapboxEventsiOS/1.0";
static NSString *const MGLMapboxEventsAPIBase = @"https://api.tiles.mapbox.com";

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

@interface MGLMapboxEvents()

// All of the following properties are written to only from
// the main thread, but can be read on any thread.
//
@property (atomic) NSString *token;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;
@property (atomic) NSString *appBuildNumber;
@property (atomic) NSString *instanceID;
@property (atomic) NSString *advertiserId;
@property (atomic) NSString *vendorId;
@property (atomic) NSString *appBundleId;
@property (atomic) NSString *userAgent;
@property (atomic) NSString *model;
@property (atomic) NSString *iOSVersion;
@property (atomic) NSString *carrier;
@property (atomic) NSUInteger flushAt;
@property (atomic) NSDateFormatter *rfc3339DateFormatter;
@property (atomic) CGFloat scale;
@property (atomic) NSURLSession *session;
@property (atomic) NSData *digicertCert;
@property (atomic) NSData *geoTrustCert;


// The paused state tracker is only ever accessed from the main thread.
//
@property (nonatomic, getter=isPaused) BOOL paused;

// The timer is only ever accessed from the main thread.
//
@property (nonatomic) NSTimer *timer;

// The flush expiration time is only ever accessed from the main thread.
//
@property (nonatomic) NSTimeInterval flushAfter;

// This is an array of events to push. All access to it will be
// from our own serial queue.
//
@property (nonatomic) NSMutableArray *eventQueue;

// This is a custom serial queue for accessing the event queue.
//
@property (nonatomic) dispatch_queue_t serialQueue;

@end

@implementation MGLMapboxEvents

// Must be called from the main thread. Only called internally.
//
- (instancetype) init {
    MGLAssertIsMainThread();

    self = [super init];
    if (self) {
        
        // Put Settings bundle into memory
        NSString *appSettingsBundle = [[NSBundle mainBundle] pathForResource:@"Settings" ofType:@"bundle"];
        if(!appSettingsBundle) {
            NSLog(@"Could not find Settings.bundle");
        } else {
            // Dynamic Settings.bundle loading based on:
            // http://stackoverflow.com/questions/510216/can-you-make-the-settings-in-settings-bundle-default-even-if-you-dont-open-the
            NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[appSettingsBundle stringByAppendingPathComponent:@"Root.plist"]];
            NSArray *preferences = [settings objectForKey:@"PreferenceSpecifiers"];
            NSMutableDictionary *defaultsToRegister = [[NSMutableDictionary alloc] initWithCapacity:[preferences count]];
            for(NSDictionary *prefSpecification in preferences) {
                NSString *key = [prefSpecification objectForKey:@"Key"];
                if(key && [[prefSpecification allKeys] containsObject:@"DefaultValue"]) {
                    [defaultsToRegister setObject:[prefSpecification objectForKey:@"DefaultValue"] forKey:key];
                }
            }
            
            [[NSUserDefaults standardUserDefaults] registerDefaults:defaultsToRegister];
        }
        _appBundleId = [[NSBundle mainBundle] bundleIdentifier];
        NSString *uniqueID = [[NSProcessInfo processInfo] globallyUniqueString];
        _serialQueue = dispatch_queue_create([[NSString stringWithFormat:@"%@.%@.events.serial", _appBundleId, uniqueID] UTF8String], DISPATCH_QUEUE_SERIAL);

        // Configure Events Infrastructure
        // ===============================

        _paused = YES;
        [self resumeMetricsCollection];
        NSBundle *resourceBundle = [NSBundle bundleWithPath:[NSBundle mgl_resourceBundlePath]];

        // Load Local Copy of Server's Public Key
        NSString *cerPath = nil;
        cerPath = [resourceBundle pathForResource:@"api_mapbox_com-geotrust" ofType:@"der"];
        if (cerPath != nil) {
            _geoTrustCert = [NSData dataWithContentsOfFile:cerPath];
        }

        cerPath = nil;
        cerPath = [resourceBundle pathForResource:@"api_mapbox_com-digicert" ofType:@"der"];
        if (cerPath != nil) {
            _digicertCert = [NSData dataWithContentsOfFile:cerPath];
        }

        // Events Control
        _eventQueue = [[NSMutableArray alloc] init];
        _flushAt = 20;
        _flushAfter = 15;
        _token = nil;
        _instanceID = [[NSUUID UUID] UUIDString];

        // Dynamic detection of ASIdentifierManager from Mixpanel
        // https://github.com/mixpanel/mixpanel-iphone/blob/master/LICENSE
        _advertiserId = @"";
        Class ASIdentifierManagerClass = NSClassFromString(@"ASIdentifierManager");
        if (ASIdentifierManagerClass) {
            SEL sharedManagerSelector = NSSelectorFromString(@"sharedManager");
            id sharedManager = ((id (*)(id, SEL))[ASIdentifierManagerClass methodForSelector:sharedManagerSelector])(ASIdentifierManagerClass, sharedManagerSelector);
            // Add check here
            SEL isAdvertisingTrackingEnabledSelector = NSSelectorFromString(@"isAdvertisingTrackingEnabled");
            BOOL trackingEnabled = ((BOOL (*)(id, SEL))[sharedManager methodForSelector:isAdvertisingTrackingEnabledSelector])(sharedManager, isAdvertisingTrackingEnabledSelector);
            if (trackingEnabled) {
                SEL advertisingIdentifierSelector = NSSelectorFromString(@"advertisingIdentifier");
                NSUUID *uuid = ((NSUUID* (*)(id, SEL))[sharedManager methodForSelector:advertisingIdentifierSelector])(sharedManager, advertisingIdentifierSelector);
                _advertiserId = [uuid UUIDString];
            }
        }
        _vendorId = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        
        _model = [self getSysInfoByName:"hw.machine"];
        _iOSVersion = [NSString stringWithFormat:@"%@ %@", [UIDevice currentDevice].systemName, [UIDevice currentDevice].systemVersion];
        if ([UIScreen instancesRespondToSelector:@selector(nativeScale)]) {
            _scale = [UIScreen mainScreen].nativeScale;
        } else {
            _scale = [UIScreen mainScreen].scale;
        }
        CTCarrier *carrierVendor = [[[CTTelephonyNetworkInfo alloc] init] subscriberCellularProvider];
        _carrier = [carrierVendor carrierName];
        
        _userAgent = MGLMapboxEventsUserAgent;
        
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
    }
    return self;
}

// Can be called from any thread. Called implicitly from any
// public class convenience methods. May return nil if this feature is disabled.
//
+ (instancetype)sharedManager {
    static dispatch_once_t onceToken;
    static MGLMapboxEvents *_sharedManager;
    dispatch_once(&onceToken, ^{
        if ( ! NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent &&
            [[NSUserDefaults standardUserDefaults] objectForKey:@"mapbox_metrics_disabled"] == nil) {
            void (^setupBlock)() = ^{
                _sharedManager = [[self alloc] init];
            };
            if ( ! [[NSThread currentThread] isMainThread]) {
                dispatch_sync(dispatch_get_main_queue(), ^{
                    setupBlock();
                });
            }
            else {
                setupBlock();
            }
        }
    });
    return _sharedManager;
}

- (void)dealloc {
    [self pauseMetricsCollection];
}

// Must be called from the main thread.
//
+ (void) setToken:(NSString *)token {
    MGLAssertIsMainThread();
    [MGLMapboxEvents sharedManager].token = token;
}

// Must be called from the main thread.
//
+ (void) setAppName:(NSString *)appName {
    MGLAssertIsMainThread();
    [MGLMapboxEvents sharedManager].appName = appName;
}

// Must be called from the main thread.
//
+ (void) setAppVersion:(NSString *)appVersion {
    MGLAssertIsMainThread();
    [MGLMapboxEvents sharedManager].appVersion = appVersion;
}

// Must be called from the main thread.
//
+ (void) setAppBuildNumber:(NSString *)appBuildNumber {
    MGLAssertIsMainThread();
    [MGLMapboxEvents sharedManager].appBuildNumber = appBuildNumber;
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
    [_session invalidateAndCancel];
    _session = nil;
    MGLMetricsLocationManager *sharedLocationManager = [MGLMetricsLocationManager sharedManager];
    [sharedLocationManager stopUpdatingLocation];
    [sharedLocationManager stopMonitoringVisits];
}

+ (void)resumeMetricsCollection {
    [[MGLMapboxEvents sharedManager] resumeMetricsCollection];
}

// Must be called from the main thread.
//
- (void)resumeMetricsCollection {
    MGLAssertIsMainThread();
    if (!self.isPaused) {
        return;
    }
    self.paused = NO;
    _session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:nil];
    MGLMetricsLocationManager *sharedLocationManager = [MGLMetricsLocationManager sharedManager];
    [sharedLocationManager startUpdatingLocation];
    [sharedLocationManager startMonitoringVisits];
}

// Can be called from any thread. Can be called rapidly from
// the UI thread, so performance is paramount.
//
+ (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        [[MGLMapboxEvents sharedManager] pushEvent:event withAttributes:attributeDictionary];
    });
}

// Can be called from any thread. Called implicitly from public
// use of +pushEvent:withAttributes:.
//
- (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary {
    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(_serialQueue, ^{
        MGLMapboxEvents *strongSelf = weakSelf;
        if ( ! strongSelf) return;
        
        // Opt Out Checking When Built
        if (![[NSUserDefaults standardUserDefaults] boolForKey:@"mapbox_metrics_enabled_preference"]) {
            [_eventQueue removeAllObjects];
            return;
        }

        // Metrics Collection Has Been Paused
        if (_paused) {
            return;
        }
        
        if (!event) return;

        NSMutableDictionary *evt = [[NSMutableDictionary alloc] initWithDictionary:attributeDictionary];
        // mapbox-events stock attributes
        [evt setObject:event forKey:@"event"];
        [evt setObject:@(1) forKey:@"version"];
        [evt setObject:[strongSelf.rfc3339DateFormatter stringFromDate:[NSDate date]] forKey:@"created"];
        [evt setObject:strongSelf.instanceID forKey:@"instance"];
        [evt setObject:strongSelf.advertiserId forKey:@"advertiserId"];
        [evt setObject:strongSelf.vendorId forKey:@"vendorId"];
        [evt setObject:strongSelf.appBundleId forKeyedSubscript:@"appBundleId"];
        
        // mapbox-events-ios stock attributes
        [evt setValue:strongSelf.model forKey:@"model"];
        [evt setValue:strongSelf.iOSVersion forKey:@"operatingSystem"];
        [evt setValue:[strongSelf getDeviceOrientation] forKey:@"orientation"];
        [evt setValue:@((int)(100 * [UIDevice currentDevice].batteryLevel)) forKey:@"batteryLevel"];
        [evt setValue:@(strongSelf.scale) forKey:@"resolution"];
        [evt setValue:strongSelf.carrier forKey:@"carrier"];
        
        NSString *cell = [strongSelf getCurrentCellularNetworkConnectionType];
        if (cell) {
            [evt setValue:cell forKey:@"cellularNetworkType"];
        } else {
            [evt setObject:[NSNull null] forKey:@"cellularNetworkType"];
        }
        
        NSString *wifi = [strongSelf getWifiNetworkName];
        if (wifi) {
            [evt setValue:wifi forKey:@"wifi"];
        } else {
            [evt setObject:[NSNull null] forKey:@"wifi"];
        }
        
        [evt setValue:@([strongSelf getContentSizeScale]) forKey:@"accessibilityFontScale"];

        // Make Immutable Version
        NSDictionary *finalEvent = [NSDictionary dictionaryWithDictionary:evt];
        
        // Put On The Queue
        [_eventQueue addObject:finalEvent];
        
        // Has Flush Limit Been Reached?
        if (_eventQueue.count >= strongSelf.flushAt) {
            [strongSelf flush];
        } else if (_eventQueue.count ==  1) {
            // If this is first new event on queue start timer,
            NSLog(@"event queue is 1, start timer.");
            [strongSelf startTimer];
        } else {
            NSLog(@"event queue is != 1, don't adjust timer.");
        }
    });
}

// Can be called from any thread.
//
+ (void) flush {
    NSLog(@"+flush called (likely timer)");
    [[MGLMapboxEvents sharedManager] flush];
}

// Can be called from any thread.
//
- (void) flush {
    if (self.token == nil) return;

    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(_serialQueue, ^{
        MGLMapboxEvents *strongSelf = weakSelf;
        if ( ! strongSelf) return;
        
        __block NSArray *events;

        NSUInteger upper = strongSelf.flushAt;
        if (strongSelf.flushAt > [_eventQueue count]) {
            if ([_eventQueue count] == 0) {
                return;
            }
            upper = [_eventQueue count];
        }
    
        // Create Array of Events to push to the Server
        NSRange theRange = NSMakeRange(0, upper);
        events = [_eventQueue subarrayWithRange:theRange];
    
        // Update Queue to remove events sent to server
        [_eventQueue removeObjectsInRange:theRange];

        // Send Array of Events to Server
        [strongSelf postEvents:events];

        // Cancel Any Timer That May Running
        if (strongSelf.timer) {
            NSLog(@"timer still exists after flush, need to clean it up.");
            [strongSelf.timer invalidate];
            strongSelf.timer = nil;
        }
    });
}

// Can be called from any thread. Called implicitly from public
// use of +flush. Posts an async network request to upload metrics.
//
- (void) postEvents:(NSArray *)events {
    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        MGLMapboxEvents *strongSelf = weakSelf;
        if (!strongSelf) return;

        // Setup URL Request
        NSString *url = [NSString stringWithFormat:@"%@/events/v1?access_token=%@", MGLMapboxEventsAPIBase, strongSelf.token];
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
        [request setValue:[strongSelf getUserAgent] forHTTPHeaderField:@"User-Agent"];
        [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPMethod:@"POST"];
        
        // Convert Array of Dictionaries to JSON
        if ([NSJSONSerialization isValidJSONObject:events]) {
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:events options:NSJSONWritingPrettyPrinted error:nil];
            [request setHTTPBody:jsonData];

            // Send non blocking HTTP Request to server
            NSURLSessionDataTask *task = [_session dataTaskWithRequest:request completionHandler: nil];
            [task resume];
        }
    });
}

// Can be called from any thread.
//
- (void) startTimer {
    void (^timerBlock)() = ^{
        // Stop Timer if it already exists
        if (_timer) {
            [_timer invalidate];
            _timer = nil;
        }

        // Start New Timer
        _timer = [NSTimer scheduledTimerWithTimeInterval:_flushAfter
                                                  target:[self class]
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
- (NSString *) getUserAgent {
    if (self.appName != nil && self.appVersion != nil && self.appBuildNumber != nil && ([self.userAgent rangeOfString:self.appName].location == NSNotFound)) {
        self.userAgent = [NSString stringWithFormat:@"%@/%@/%@ %@", self.appName, self.appVersion, self.appBuildNumber, self.userAgent];
    }
    return self.userAgent;
}

// Can be called from any thread.
//
- (NSString *) formatDate:(NSDate *)date {
    return [self.rfc3339DateFormatter stringFromDate:date];
}

// Can be called from any thread.
//
- (NSString *) getDeviceOrientation {
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
- (NSInteger) getContentSizeScale {
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
- (NSString *)getSysInfoByName:(char *)typeSpecifier
{
    size_t size;
    sysctlbyname(typeSpecifier, NULL, &size, NULL, 0);
    
    char *answer = malloc(size);
    sysctlbyname(typeSpecifier, answer, &size, NULL, 0);
    
    NSString *results = [NSString stringWithCString:answer encoding: NSUTF8StringEncoding];
    
    free(answer);
    return results;
}

// Can be called from any thread.
//
- (NSString *) getWifiNetworkName {
    
    NSString *ssid = nil;
    CFArrayRef interfaces = CNCopySupportedInterfaces();
    if (interfaces) {
        NSDictionary *info = CFBridgingRelease(CNCopyCurrentNetworkInfo(CFArrayGetValueAtIndex(interfaces, 0)));
        if (info) {
            ssid = info[@"SSID"];
        }
        CFRelease(interfaces);
    }
    
    return ssid;
}

// Can be called from any thread.
//
- (NSString *) getCurrentCellularNetworkConnectionType {
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

            BOOL found = false;
            // Try GeoTrust Cert First
            for (int lc = 0; lc < numKeys; lc++) {
                SecCertificateRef certificate = SecTrustGetCertificateAtIndex(serverTrust, lc);
                NSData *remoteCertificateData = CFBridgingRelease(SecCertificateCopyData(certificate));

                // Compare Remote Key With Local Version
                if ([remoteCertificateData isEqualToData:_geoTrustCert]) {
                    // Found the certificate; continue connecting
                    completionHandler(NSURLSessionAuthChallengeUseCredential, [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust]);
                    found = true;
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
                        found = true;
                        break;
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
