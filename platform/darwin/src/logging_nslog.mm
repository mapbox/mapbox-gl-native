#include <mbgl/util/logging.hpp>
#include <mbgl/util/enum.hpp>

#import <Foundation/Foundation.h>

namespace mbgl {
    
    void Log::platformRecord(EventSeverity severity, const std::string &msg) {
        NSString *message =
        [[NSString alloc] initWithBytes:msg.data() length:msg.size() encoding:NSUTF8StringEncoding];
        NSLog(@"[%s] %@", Enum<EventSeverity>::toString(severity), message);
    }
    
}
