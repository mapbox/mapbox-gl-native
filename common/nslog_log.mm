#include "nslog_log.hpp"

#import <Foundation/Foundation.h>

namespace llmr {

void NSLogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    NSLog(@"[%s] %s: %@", stringifyEventSeverity(severity), stringifyEvent(event),
          [[NSString alloc] initWithBytes:msg.data()
                                   length:msg.size()
                                 encoding:NSUTF8StringEncoding]);
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
