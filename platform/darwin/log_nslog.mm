#include <mbgl/platform/darwin/log_nslog.hpp>

#import <Foundation/Foundation.h>

#include <cstdarg>

namespace mbgl {

void NSLogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    NSString *message =
        [[NSString alloc] initWithBytes:msg.data() length:msg.size() encoding:NSUTF8StringEncoding];
    NSLog(@"[%s] %s: %@", EventSeverityClass(severity).c_str(), EventClass(event).c_str(), message);
    [message release];
}

void NSLogBackend::record(EventSeverity severity, Event event, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const size_t len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    std::unique_ptr<char[]> buffer(new char[len + 1]);
    va_start(args, format);
    vsnprintf(buffer.get(), len + 1, format, args);
    va_end(args);
    NSLog(@"[%s] %s: %s", EventSeverityClass(severity).c_str(), EventClass(event).c_str(),
          buffer.get());
}

void NSLogBackend::record(EventSeverity severity, Event event, int64_t code) {
    NSLog(@"[%s] %s: (%lld)", EventSeverityClass(severity).c_str(), EventClass(event).c_str(),
          code);
}

void NSLogBackend::record(EventSeverity severity, Event event, int64_t code,
                          const std::string &msg) {
    NSString *message =
        [[NSString alloc] initWithBytes:msg.data() length:msg.size() encoding:NSUTF8StringEncoding];
    NSLog(@"[%s] %s: (%lld) %@", EventSeverityClass(severity).c_str(), EventClass(event).c_str(),
          code, message);
    [message release];
}

}
