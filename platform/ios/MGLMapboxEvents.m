#import "MGLMapboxEvents.h"

#import <UIKit/UIKit.h>
#import <SystemConfiguration/CaptiveNetwork.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>

#import "MGLMetricsLocationManager.h"

#include <sys/sysctl.h>

static NSString *const MGLMapboxEventsUserAgent = @"MapboxEventsiOS/1.0";
static NSString *const MGLMapboxEventsAPIBase = @"https://api.tiles.mapbox.com";

NSString *const MGLEventTypeMapLoad = @"map.load";
NSString *const MGLEventTypeMapTap = @"map.click";
NSString *const MGLEventTypeMapDragEnd = @"map.dragend";
NSString *const MGLEventTypeLocation = @"Location";

NSString *const MGLEventKeyLatitude = @"lat";
NSString *const MGLEventKeyLongitude = @"lng";
NSString *const MGLEventKeyZoomLevel = @"zoom";
NSString *const MGLEventKeyPushEnabled = @"enabled.push";
NSString *const MGLEventKeyEmailEnabled = @"enabled.email";
NSString *const MGLEventKeyGestureID = @"gesture";

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
// dispatches will use a `weakSelf` to avoid cyclical
// strong references.
//

@interface MGLMapboxEvents()

// All of the following properties are written to only from
// the main thread, but can be read on any thread.
//
@property (atomic) NSString *token;
@property (atomic) NSString *appName;
@property (atomic) NSString *appVersion;
@property (atomic) NSString *instanceID;
@property (atomic) NSString *anonID;
@property (atomic) NSString *userAgent;
@property (atomic) NSString *model;
@property (atomic) NSString *iOSVersion;
@property (atomic) NSString *carrier;
@property (atomic) NSUInteger flushAt;
@property (atomic) NSDateFormatter *rfc3339DateFormatter;
@property (atomic) CGFloat scale;

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
    assert([[NSThread currentThread] isMainThread]);

    self = [super init];
    if (self) {
        
        // Put Settings bundle into memory
        NSString *settingsBundle = [[NSBundle mainBundle] pathForResource:@"Settings" ofType:@"bundle"];
        if(!settingsBundle) {
            NSLog(@"Could not find Settings.bundle");
        } else {
            // Dynamic Settings.bundle loading based on:
            // http://stackoverflow.com/questions/510216/can-you-make-the-settings-in-settings-bundle-default-even-if-you-dont-open-the
            NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[settingsBundle stringByAppendingPathComponent:@"Root.plist"]];
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
        NSString *bundleID = [[NSBundle mainBundle] bundleIdentifier];
        NSString *uniqueID = [[NSProcessInfo processInfo] globallyUniqueString];
        _serialQueue = dispatch_queue_create([[NSString stringWithFormat:@"%@.%@.events.serial", bundleID, uniqueID] UTF8String], DISPATCH_QUEUE_SERIAL);

        // Configure Events Infrastructure
        _eventQueue = [[NSMutableArray alloc] init];
        _flushAt = 20;
        _flushAfter = 60;
        _token = nil;
        _instanceID = [[NSUUID UUID] UUIDString];
        // Dynamic detection of ASIdentifierManager from Mixpanel
        // https://github.com/mixpanel/mixpanel-iphone/blob/master/LICENSE
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
                _anonID = [uuid UUIDString];
            } else {
                _anonID = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
            }
        } else {
            _anonID = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        }
        
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
        [_rfc3339DateFormatter setDateFormat:@"yyyy'-'MM'-'dd'T'HH':'mm':'ss'Z'"];
        [_rfc3339DateFormatter setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
    }
    return self;
}

// Can be called from any thread. Called implicitly from any
// public class convenience methods.
//
+ (instancetype)sharedManager {
    static dispatch_once_t onceToken;
    static MGLMapboxEvents *_sharedManager;
    dispatch_once(&onceToken, ^{
        void (^setupBlock)() = ^{
            _sharedManager = [[self alloc] init];
            // setup dedicated location manager on first use
            [MGLMetricsLocationManager sharedManager];
        };
        if ( ! [[NSThread currentThread] isMainThread]) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                setupBlock();
            });
        } else {
            setupBlock();
        }
    });
    return _sharedManager;
}

// Must be called from the main thread.
//
+ (void) setToken:(NSString *)token {
    assert([[NSThread currentThread] isMainThread]);
    [MGLMapboxEvents sharedManager].token = token;
}

// Must be called from the main thread.
//
+ (void) setAppName:(NSString *)appName {
    assert([[NSThread currentThread] isMainThread]);
    [MGLMapboxEvents sharedManager].appName = appName;
}

// Must be called from the main thread.
//
+ (void) setAppVersion:(NSString *)appVersion {
    assert([[NSThread currentThread] isMainThread]);
    [MGLMapboxEvents sharedManager].appVersion = appVersion;
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
        // Opt Out Checking When Built
        if (![[NSUserDefaults standardUserDefaults] boolForKey:@"mapbox_metrics_enabled_preference"]) {
            [_eventQueue removeAllObjects];
            return;
        }

        // Add Metrics Disabled App Wide Check
        if ([[NSUserDefaults standardUserDefaults] objectForKey:@"mapbox_metrics_disabled"] != nil) {
            [_eventQueue removeAllObjects];
            return;
        }
        
        if (!event) return;

        NSMutableDictionary *evt = [[NSMutableDictionary alloc] initWithDictionary:attributeDictionary];
        // mapbox-events stock attributes
        [evt setObject:event forKey:@"event"];
        [evt setObject:@(1) forKey:@"version"];
        [evt setObject:[weakSelf formatDate:[NSDate date]] forKey:@"created"];
        [evt setObject:weakSelf.instanceID forKey:@"instance"];
        [evt setObject:weakSelf.anonID forKey:@"anonid"];
        
        // mapbox-events-ios stock attributes
        [evt setValue:[weakSelf.rfc3339DateFormatter stringFromDate:[NSDate date]] forKey:@"created"];
        [evt setValue:weakSelf.model forKey:@"model"];
        [evt setValue:weakSelf.iOSVersion forKey:@"operatingSystem"];
        [evt setValue:[weakSelf getDeviceOrientation] forKey:@"orientation"];
        [evt setValue:@(100 * [UIDevice currentDevice].batteryLevel) forKey:@"batteryLevel"];
        [evt setValue:@(weakSelf.scale) forKey:@"resolution"];
        [evt setValue:weakSelf.carrier forKey:@"carrier"];
        [evt setValue:[weakSelf getCurrentCellularNetworkConnectionType] forKey:@"cellularNetworkType"];
        [evt setValue:[weakSelf getWifiNetworkName] forKey:@"wifi"];
        [evt setValue:@([weakSelf getContentSizeScale]) forKey:@"accessibilityFontScale"];

        // Make Immutable Version
        NSDictionary *finalEvent = [NSDictionary dictionaryWithDictionary:evt];
        
        // Put On The Queue
        [_eventQueue addObject:finalEvent];
        
        // Has Flush Limit Been Reached?
        if (_eventQueue.count >= weakSelf.flushAt) {
            [weakSelf flush];
        }
        
        // Reset Timer (Initial Starting of Timer after first event is pushed)
        [weakSelf startTimer];
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
    if (self.token == nil) return;

    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(_serialQueue, ^{
        __block NSArray *events;

        NSUInteger upper = weakSelf.flushAt;
        if (weakSelf.flushAt > [_eventQueue count]) {
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
        [weakSelf postEvents:events];
    });
}

// Can be called from any thread. Called implicitly from public
// use of +flush. Posts an async network request to upload metrics.
//
- (void) postEvents:(NSArray *)events {
    __weak MGLMapboxEvents *weakSelf = self;

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        // Setup URL Request
        NSString *url = [NSString stringWithFormat:@"%@/events/v1?access_token=%@", MGLMapboxEventsAPIBase, weakSelf.token];
        NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
        [request setValue:[weakSelf getUserAgent] forHTTPHeaderField:@"User-Agent"];
        [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
        [request setHTTPMethod:@"POST"];
        
        // Convert Array of Dictionaries to JSON
        if ([NSJSONSerialization isValidJSONObject:events]) {
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:events options:NSJSONWritingPrettyPrinted error:nil];
            [request setHTTPBody:jsonData];

            // Send non blocking HTTP Request to server
            [NSURLConnection sendAsynchronousRequest:request
                                               queue:nil
                                   completionHandler:nil];
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
    if (self.appName != nil && self.appVersion != nil && ([self.userAgent rangeOfString:self.appName].location == NSNotFound)) {
        self.userAgent = [NSString stringWithFormat:@"%@/%@ %@", self.appName, self.appVersion, self.userAgent];
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
    
    NSString *ssid = @"";
    CFArrayRef interfaces = CNCopySupportedInterfaces();
    if (interfaces) {
        NSDictionary *info = (__bridge NSDictionary *)CNCopyCurrentNetworkInfo(CFArrayGetValueAtIndex(interfaces, 0));
        if (info) {
            ssid = info[@"SSID"];
        } else {
            ssid = @"NONE";
        }
    } else {
        ssid = @"NONE";
    }
    
    return ssid;
}

// Can be called from any thread.
//
- (NSString *) getCurrentCellularNetworkConnectionType {
    CTTelephonyNetworkInfo *telephonyInfo = [CTTelephonyNetworkInfo new];
    NSString *radioTech = telephonyInfo.currentRadioAccessTechnology;
    
    if (radioTech == nil) {
        return @"NONE";
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
+ (NSString *) checkEmailEnabled {
    __block NSString *result;

    NSString *(^mailCheckBlock)(void) = ^{
        NSString *email = @"Unknown";
        Class MFMailComposeViewController = NSClassFromString(@"MFMailComposeViewController");
        if (MFMailComposeViewController) {
            SEL canSendMail = NSSelectorFromString(@"canSendMail");
            BOOL sendMail = ((BOOL (*)(id, SEL))[MFMailComposeViewController methodForSelector:canSendMail])
            (MFMailComposeViewController, canSendMail);
            email = [NSString stringWithFormat:@"%i", sendMail];
        }
        return email;
    };

    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            result = mailCheckBlock();
        });
    } else {
        result = mailCheckBlock();
    }

    return result;
}

// Can be called from any thread.
//
+ (BOOL) checkPushEnabled {
    __block BOOL result;

    BOOL (^pushCheckBlock)(void) = ^{
        if ([[UIApplication sharedApplication] respondsToSelector:@selector(isRegisteredForRemoteNotifications)]) {
            // iOS 8+
            result = [[UIApplication sharedApplication] isRegisteredForRemoteNotifications];
        } else {
            // iOS 7
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            UIRemoteNotificationType types = [[UIApplication sharedApplication] enabledRemoteNotificationTypes];
            result = (types == UIRemoteNotificationTypeNone) ? NO : YES;
#pragma clang diagnostic pop
        }
        return result;
    };

    if ( ! [[NSThread currentThread] isMainThread]) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            result = pushCheckBlock();
        });
    } else {
        result = pushCheckBlock();
    }

    return result;
}

@end
