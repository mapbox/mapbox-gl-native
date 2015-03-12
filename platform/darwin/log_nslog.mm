#include <mbgl/platform/log.hpp>

#import <Foundation/Foundation.h>

namespace mbgl {

void Log::platformRecord(EventSeverity severity, Event event, int64_t code,
                         const std::string &msg) {
    NSString *message =
        [[NSString alloc] initWithBytes:msg.data() length:msg.size() encoding:NSUTF8StringEncoding];
    NSLog(@"[%s] %s: (%lld) %@", EventSeverityClass(severity).c_str(), EventClass(event).c_str(),
          code, message);
}

}
