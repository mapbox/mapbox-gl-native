#include <mbgl/util/logging.hpp>
#include <mbgl/util/enum.hpp>

#import <Foundation/Foundation.h>
#if __has_builtin(__builtin_os_log_format)
#import <os/log.h>
#endif

namespace mbgl {
    
void Log::platformRecord(EventSeverity severity, const std::string &msg) {
    NSString *message =
    [[NSString alloc] initWithBytes:msg.data() length:msg.size() encoding:NSUTF8StringEncoding];
    if (@available(iOS 10.0, macOS 10.12.0, *)) {
        static dispatch_once_t once;
        static os_log_t info_log;
        static os_log_t debug_log;
        static os_log_t error_log;
        
        dispatch_once(&once, ^ {
            info_log = os_log_create("com.mapbox.Mapbox", "INFO");
            debug_log = os_log_create("com.mapbox.Mapbox", "DEBUG");
            error_log = os_log_create("com.mapbox.Mapbox", "ERROR");
        });
        
        os_log_t mapbox_log;
        os_log_type_t logType;
        switch (severity) {
            case EventSeverity:: Info:
            case EventSeverity:: Warning:
            {
                mapbox_log = info_log;
                logType = OS_LOG_TYPE_INFO;
                break;
            }
            case EventSeverity:: Debug:
            {
                mapbox_log = debug_log;
                logType = OS_LOG_TYPE_DEBUG;
                break;
            }
            case EventSeverity:: Error:
            {
                mapbox_log = error_log;
                logType = OS_LOG_TYPE_ERROR;
                break;
            }
        }
        os_log_with_type(mapbox_log, logType, "[%s]: %@", Enum<EventSeverity>::toString(severity), message);
    }
    else {
        NSLog(@"[%s] %@", Enum<EventSeverity>::toString(severity), message);
    }
}
    
}
