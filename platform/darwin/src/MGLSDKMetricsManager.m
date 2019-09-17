#import "MGLSDKMetricsManager_Private.h"
#import "MGLNetworkConfiguration_Private.h"
#if TARGET_OS_IOS
#import "MGLMapboxEvents.h"
#import <mach-o/arch.h>
#import <sys/utsname.h>
#import <UIKit/UIKit.h>
#endif

NSString* MGLStringFromMetricType(MGLMetricType metricType) {
    NSString *eventName;
    
    switch (metricType) {
        case MGLMetricTypePerformance:
            eventName = kMGLDownloadPerformanceEvent;
            break;
    }
    return eventName;
}

@interface MGLMetricsManager() <MGLNetworkConfigurationMetricsDelegate>

@property (strong, nonatomic) NSDictionary *metadata;

@end

@implementation MGLMetricsManager

+ (instancetype)sharedManager
{
    static dispatch_once_t once;
    static MGLMetricsManager *sharedConfiguration;
    dispatch_once(&once, ^{
        sharedConfiguration = [[self alloc] init];
        [MGLNetworkConfiguration sharedManager].metricsDelegate = sharedConfiguration;
#if TARGET_OS_IOS
        UIDevice *currentDevice = [UIDevice currentDevice];
        
        NSString *osVersion = currentDevice.systemVersion;
        
        NSString *screenSize = [NSString stringWithFormat:@"%.fx%.f", [UIScreen mainScreen].bounds.size.width,
                                [UIScreen mainScreen].bounds.size.height];
        
        NSLocale *currentLocale = [NSLocale currentLocale];
        
        NSString *country = [currentLocale objectForKey:NSLocaleCountryCode] ?: @"unknown";
        
        NSString *device = deviceName();
        
        NSString *abi = @"unknown";
        
        {
            const NXArchInfo *localArchInfo = NXGetLocalArchInfo();
        
            if (localArchInfo) {
                abi = @(localArchInfo->description);
                NXFreeArchInfo(localArchInfo);
            }
        }
        
        NSString *ram = [NSString stringWithFormat:@"%llu", [NSProcessInfo processInfo].physicalMemory];
        
        NSString *os = currentDevice.systemName;
        
        sharedConfiguration.metadata = @{ @"version" : osVersion,
                                          @"screenSize" : screenSize,
                                          @"country" : country,
                                          @"device" : device,
                                          @"abi" : abi,
                                          @"brand" : @"Apple",
                                          @"ram" : ram,
                                          @"os" : os
                                          };
#endif
    });
    return sharedConfiguration;
}

- (void)handleMetricsEvent:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    if ([self.delegate metricsManager:self shouldHandleMetric:metricType]) {
        [self.delegate metricsManager:self didCollectMetric:metricType withAttributes:attributes];
    }
}

#if TARGET_OS_IOS
- (void)pushMetric:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    NSString *eventName = MGLStringFromMetricType(metricType);
    NSMutableDictionary *mutableAttributes = [NSMutableDictionary dictionaryWithDictionary:attributes];
    [mutableAttributes setObject:self.metadata forKey:@"metadata"];
    
    [MGLMapboxEvents pushEvent:eventName withAttributes:mutableAttributes];
}

NSString* deviceName()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}
#endif

- (void)networkConfiguration:(MGLNetworkConfiguration *)networkConfiguration didGenerateMetricEvent:(NSDictionary *)metricEvent {
    [self handleMetricsEvent:MGLMetricTypePerformance withAttributes:metricEvent];
}

@end
