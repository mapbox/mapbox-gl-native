#import "MGLLoggingConfiguration_Private.h"

#if __has_builtin(__builtin_os_log_format)
#import <os/log.h>
#endif

@implementation MGLLoggingConfiguration

+ (instancetype)sharedConfiguration
{
    static dispatch_once_t once;
    static id sharedConfiguration;
    dispatch_once(&once, ^{
        sharedConfiguration = [[self alloc] init];
        ((MGLLoggingConfiguration *)sharedConfiguration).handler = nil;
    });
    return sharedConfiguration;
}

- (void)setHandler:(void (^)(MGLLoggingLevel, NSString *, NSUInteger, NSString *))handler {
    
    if (!handler) {
        _handler = [self defaultBlockHandler];
    } else {
        _handler = handler;
    }
}

- (void)logCallingFunction:(const char *)callingFunction functionLine:(NSUInteger)functionLine messageType:(MGLLoggingLevel)type format:(id)messageFormat, ...
{
    va_list formatList;
    va_start(formatList, messageFormat);
    NSString *formattedMessage = [[NSString alloc] initWithFormat:messageFormat arguments:formatList];
    va_end(formatList);
    
    _handler(type, @(callingFunction), functionLine, formattedMessage);
    
}

- (MGLLoggingBlockHandler)defaultBlockHandler {
    MGLLoggingBlockHandler mapboxHandler = ^(MGLLoggingLevel level, NSString *fileName, NSUInteger line, NSString *message) {
        
        if (@available(iOS 10.0, macOS 10.12.0, *)) {
            static dispatch_once_t once;
            static os_log_t mapbox_log;
            static os_log_type_t log_types[] = { OS_LOG_TYPE_DEFAULT,
                                                    OS_LOG_TYPE_INFO,
                                                    OS_LOG_TYPE_DEBUG,
                                                    OS_LOG_TYPE_ERROR,
                                                    OS_LOG_TYPE_FAULT };
            dispatch_once(&once, ^ {
                mapbox_log = os_log_create("com.mapbox.maps-ios-sdk", "SDK");
            });
            NSUInteger logTypesCount = sizeof(log_types) / sizeof(os_log_type_t);
            NSAssert(level <= logTypesCount, @"There is an attempt to log a non suported logging level.");
            os_log_type_t logType = log_types[level];
            os_log_with_type(mapbox_log, logType, "%@ - %d: %@", fileName, line, message);
        } else {
            NSLog(@"[SDK] %@ - %lu: %@", fileName, line, message);
        }
    };
    
    return mapboxHandler;
}

@end
