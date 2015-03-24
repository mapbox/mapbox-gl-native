//
//  MapboxEvents.m
//  MapboxEvents
//
//  Dynamic detection of ASIdentifierManager from Mixpanel
//  https://github.com/mixpanel/mixpanel-iphone/blob/master/LICENSE
//
//  Created by Brad Leege on 3/5/15.
//  Copyright (c) 2015 Mapbox. All rights reserved.
//

#import "MGLMapboxEvents.h"
#import <UIKit/UIKit.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>
#include <sys/sysctl.h>
#import <SystemConfiguration/CaptiveNetwork.h>

@interface MGLMapboxEvents()

@property (atomic) NSMutableArray *queue;
@property (atomic) NSString *instance;
@property (atomic) NSString *anonid;
@property (atomic) NSTimer *timer;
@property (atomic) NSString *userAgent;

@end

@implementation MGLMapboxEvents

static MGLMapboxEvents *sharedManager = nil;

NSDateFormatter *rfc3339DateFormatter = nil;
NSString *model;
NSString *iOSVersion;
NSString *carrier;
NSNumber *scale;

- (id) init {
    self = [super init];
    if (self) {
        
        // Put Settings bundle into memory
        NSString *settingsBundle = [[NSBundle mainBundle] pathForResource:@"Settings" ofType:@"bundle"];
        if(!settingsBundle) {
            NSLog(@"Could not find Settings.bundle");
        } else {
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
        
        // Configure Events Infrastructure
        _queue = [[NSMutableArray alloc] init];
        _flushAt = 20;
        _flushAfter = 10000;
        _api = @"https://api.tiles.mapbox.com";
        _token = nil;
        _instance = [[NSUUID UUID] UUIDString];
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
                _anonid = [uuid UUIDString];
            } else {
                _anonid = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
            }
        } else {
            _anonid = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        }
        
        model = [self getSysInfoByName:"hw.machine"];
        iOSVersion = [NSString stringWithFormat:@"%@ %@", [UIDevice currentDevice].systemName, [UIDevice currentDevice].systemVersion];
        if ([UIScreen instancesRespondToSelector:@selector(nativeScale)]) {
            scale = [[NSNumber alloc] initWithFloat:[UIScreen mainScreen].nativeScale];
        } else {
            scale = [[NSNumber alloc] initWithFloat:[UIScreen mainScreen].scale];
        }
        CTCarrier *carrierVendor = [[[CTTelephonyNetworkInfo alloc] init] subscriberCellularProvider];
        carrier = [carrierVendor carrierName];
        
        _userAgent = @"MapboxEventsiOS/1.0";
        
        // Setup Date Format
        rfc3339DateFormatter = [[NSDateFormatter alloc] init];
        NSLocale *enUSPOSIXLocale = [[NSLocale alloc] initWithLocaleIdentifier:@"en_US_POSIX"];
        
        [rfc3339DateFormatter setLocale:enUSPOSIXLocale];
        [rfc3339DateFormatter setDateFormat:@"yyyy'-'MM'-'dd'T'HH':'mm':'ss'Z'"];
        [rfc3339DateFormatter setTimeZone:[NSTimeZone timeZoneForSecondsFromGMT:0]];
    }
    return self;
}

+ (id)sharedManager {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedManager = [[self alloc] init];
    });
    return sharedManager;
}

- (void) pushEvent:(NSString *)event withAttributes:(NSDictionary *)attributeDictionary {
    
    // TODO Add Opt Out Checking When Built
    /**
     if (![[NSUserDefaults standardUserDefaults] boolForKey:@"mapbox_metrics_enabled_preference"]) {
     NSLog(@"Mapbox Metrics are not enabled, so return without sending in data.");
     return;
     }
     */

    // TODO - Add Metrics Disabled App Wide Check
    
    if (!event) {
        return;
    }
    
    NSMutableDictionary *evt = [[NSMutableDictionary alloc] init];
    // mapbox-events stock attributes
    [evt setObject:event forKey:@"event"];
    [evt setObject:[NSNumber numberWithInt:1] forKey:@"version"];
    [evt setObject:[self formatDate:[NSDate date]] forKey:@"created"];
    [evt setObject:self.instance forKey:@"instance"];
    [evt setObject:self.anonid forKey:@"anonid"];
    
    // mapbox-events-ios stock attributes
    [evt setValue:[rfc3339DateFormatter stringFromDate:[NSDate date]] forKey:@"deviceTimestamp"];
    [evt setValue:model forKey:@"model"];
    [evt setValue:iOSVersion forKey:@"operatingSystem"];
    [evt setValue:[self getDeviceOrientation] forKey:@"orientation"];
    [evt setValue:[[NSNumber alloc] initWithFloat:(100 * [UIDevice currentDevice].batteryLevel)] forKey:@"batteryLevel"];
    [evt setValue:scale forKey:@"resolution"];
    [evt setValue:carrier forKey:@"carrier"];
    [evt setValue:[self getWifiNetworkName] forKey:@"wifi"];
    [evt setValue:[NSNumber numberWithInt:[self getContentSizeScale]] forKey:@"accessibilityFontScale"];
    
    for (NSString *key in [attributeDictionary allKeys]) {
        [evt setObject:[attributeDictionary valueForKey:key] forKey:key];
    }
    
    // Make Immutable Version
    NSDictionary *finalEvent = [NSDictionary dictionaryWithDictionary:evt];
    
    // Put On The Queue
    [self.queue addObject:finalEvent];
    
    // Has Flush Limit Been Reached?
    if ((int)_queue.count >= (int)_flushAt) {
        [self flush];
    }
    
    // Reset Timer (Initial Starting of Timer after first event is pushed)
    [self startTimer];
}

- (void) flush {
    if (_token == nil) {
        NSLog(@"token hasn't been set yet, so no events can be sent. return");
        return;
    }
    
    
    int upper = (int)_flushAt;
    if (_flushAt > [_queue count]) {
        if ([_queue count] == 0) {
            return;
        }
        upper = (int)[_queue count];
    }
    
    // Create Array of Events to push to the Server
    NSRange theRange = NSMakeRange(0, upper);
    NSArray *events = [_queue subarrayWithRange:theRange];
    
    // Update Queue to remove events sent to server
    [_queue removeObjectsInRange:theRange];
    
    // Send Array of Events to Server
    [self postEvents:events];
    
}

- (void) postEvents:(NSArray *)events {
    // Setup URL Request
    NSString *url = [NSString stringWithFormat:@"%@/events/v1?access_token=%@", _api, _token];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:url]];
    [request setValue:[self getUserAgent] forHTTPHeaderField:@"User-Agent"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [request setHTTPMethod:@"POST"];
    
    // Convert Array of Dictionaries to JSON
    if ([NSJSONSerialization isValidJSONObject:events]) {
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:events options:NSJSONWritingPrettyPrinted error:nil];
        [request setHTTPBody:jsonData];
        
        // Send non blocking HTTP Request to server
        [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:nil];
    }
}

- (void) startTimer {
    // Stop Timer if it already exists
    if (_timer) {
        [_timer invalidate];
        _timer = nil;
    }
    
    // Start New Timer
    NSTimeInterval interval = (double)((NSInteger)_flushAfter);
    _timer = [NSTimer scheduledTimerWithTimeInterval:interval target:self selector:@selector(flush) userInfo:nil repeats:YES];
}

- (NSString *) getUserAgent {
    
    if (_appName != nil && _appVersion != nil && ([_userAgent rangeOfString:_appName].location == NSNotFound)) {
        _userAgent = [NSString stringWithFormat:@"%@/%@ %@", _appName, _appVersion, _userAgent];
    }
    return _userAgent;
}

- (NSString *) formatDate:(NSDate *)date {
    return [rfc3339DateFormatter stringFromDate:date];
}

- (NSString *) getDeviceOrientation {
    switch ([UIDevice currentDevice].orientation) {
        case UIDeviceOrientationUnknown:
            return @"Unknown";
            break;
        case UIDeviceOrientationPortrait:
            return @"Portrait";
            break;
        case UIDeviceOrientationPortraitUpsideDown:
            return @"PortraitUpsideDown";
            break;
        case UIDeviceOrientationLandscapeLeft:
            return @"LandscapeLeft";
            break;
        case UIDeviceOrientationLandscapeRight:
            return @"LandscapeRight";
            break;
        case UIDeviceOrientationFaceUp:
            return @"FaceUp";
            break;
        case UIDeviceOrientationFaceDown:
            return @"FaceDown";
            break;
        default:
            return @"Default - Unknown";
            break;
    }
}

- (int) getContentSizeScale {
    NSString *sc = [UIApplication sharedApplication].preferredContentSizeCategory;
    
    if ([sc isEqualToString:UIContentSizeCategoryExtraSmall]) {
        return -3;
    } else if ([sc isEqualToString:UIContentSizeCategorySmall]) {
        return -2;
    } else if ([sc isEqualToString:UIContentSizeCategoryMedium]) {
        return -1;
    } else if ([sc isEqualToString:UIContentSizeCategoryLarge]) {
        return 0;
    } else if ([sc isEqualToString:UIContentSizeCategoryExtraLarge]) {
        return 1;
    } else if ([sc isEqualToString:UIContentSizeCategoryExtraExtraLarge]) {
        return 2;
    } else if ([sc isEqualToString:UIContentSizeCategoryExtraExtraExtraLarge]) {
        return 3;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityMedium]) {
        return -11;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityLarge]) {
        return 10;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityExtraLarge]) {
        return 11;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityExtraExtraLarge]) {
        return 12;
    } else if ([sc isEqualToString:UIContentSizeCategoryAccessibilityExtraExtraExtraLarge]) {
        return 13;
    }
    return -9999;
}


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

- (NSString *) getWifiNetworkName {
    
    NSString *ssid = @"";
    CFArrayRef interfaces = CNCopySupportedInterfaces();
    if (interfaces) {
        NSDictionary *info = (__bridge NSDictionary *)CNCopyCurrentNetworkInfo(CFArrayGetValueAtIndex(interfaces, 0));
        if (info) {
            ssid = info[@"SSID"];
        } else {
            ssid = @"<<NONE>>";
        }
    } else {
        ssid = @"<<NONE>>";
    }
    
    return ssid;
}


@end