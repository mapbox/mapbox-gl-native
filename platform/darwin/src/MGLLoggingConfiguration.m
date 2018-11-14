#import "MGLLoggingConfiguration_Private.h"

#ifndef MGL_LOGGING_DISABLED
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
            static os_log_t info_log;
#if MGL_LOGGING_ENABLE_DEBUG
            static os_log_t debug_log;
#endif
            static os_log_t error_log;
            static os_log_t fault_log;
            static os_log_type_t log_types[] = { OS_LOG_TYPE_DEFAULT,
                                                    OS_LOG_TYPE_INFO,
#if MGL_LOGGING_ENABLE_DEBUG
                                                    OS_LOG_TYPE_DEBUG,
#endif
                                                    OS_LOG_TYPE_ERROR,
                                                    OS_LOG_TYPE_FAULT };
            dispatch_once(&once, ^ {
                info_log = os_log_create("com.mapbox.maps", "INFO");
#if MGL_LOGGING_ENABLE_DEBUG
                debug_log = os_log_create("com.mapbox.maps", "DEBUG");
#endif
                error_log = os_log_create("com.mapbox.maps", "ERROR");
                fault_log = os_log_create("com.mapbox.maps", "FAULT");
            });
            
            os_log_t mapbox_log;
            switch (level) {
                case MGLLoggingLevelInfo:
                    mapbox_log = info_log;
                    break;
#if MGL_LOGGING_ENABLE_DEBUG
                case MGLLoggingLevelDebug:
                    mapbox_log = debug_log;
                    break;
#endif
                case MGLLoggingLevelError:
                    mapbox_log = error_log;
                    break;
                case MGLLoggingLevelFault:
                    mapbox_log = fault_log;
                    break;
                case MGLLoggingLevelNone:
                default:
                    break;
            }

            os_log_type_t logType = log_types[level];
            os_log_with_type(mapbox_log, logType, "%@ - %lu: %@", fileName, (unsigned long)line, message);
        } else {
            NSString *category;
            switch (level) {
                case MGLLoggingLevelInfo:
                    category = @"INFO";
                    break;
#if MGL_LOGGING_ENABLE_DEBUG
                case MGLLoggingLevelDebug:
                    category = @"DEBUG";
                    break;
#endif
                case MGLLoggingLevelError:
                    category = @"ERROR";
                    break;
                case MGLLoggingLevelFault:
                    category = @"FAULT";
                    break;
                case MGLLoggingLevelNone:
                default:
                    break;
            }
            
            NSLog(@"[%@] %@ - %lu: %@", category, fileName, (unsigned long)line, message);
        }
    };
    
    return mapboxHandler;
}

@end
#endif
