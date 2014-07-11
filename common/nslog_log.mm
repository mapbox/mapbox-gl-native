#include "nslog_log.hpp"

#import <Foundation/Foundation.h>

namespace llmr {

void NSLogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    NSLog(@"[%s] %s: %@", stringifyEventSeverity(severity), stringifyEvent(event),
          [[NSString alloc] initWithBytes:msg.data()
                                   length:msg.size()
                                 encoding:NSUTF8StringEncoding]);
}


void NSLogBackend::record(EventSeverity severity, Event event, const char* format, ...) {
    va_list args;
    va_start(args, format);
    const int len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    std::unique_ptr<char[]> buffer(new char[len + 1]);
    va_start(args, format);
    vsnprintf(buffer.get(), len + 1, format, args);
    va_end(args);
    NSLog(@"[%s] %s: %s", stringifyEventSeverity(severity), stringifyEvent(event), buffer.get());
}

void NSLogBackend::record(EventSeverity severity, Event event, int64_t code) {
    NSLog(@"[%s] %s: (%lld)", stringifyEventSeverity(severity), stringifyEvent(event), code);
}

void NSLogBackend::record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    NSLog(@"[%s] %s: (%lld) %@", stringifyEventSeverity(severity), stringifyEvent(event), code,
          [[NSString alloc] initWithBytes:msg.data()
                                   length:msg.size()
                                 encoding:NSUTF8StringEncoding]);
}
}
