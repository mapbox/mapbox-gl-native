#include <mbgl/platform/log.hpp>

#import <Foundation/Foundation.h>

namespace mbgl {

void Log::platformRecord(EventSeverity severity, const std::string &msg) {
    if (severity == EventSeverity::Error) {
        [NSException raise:@"Mapbox GL Error" format:@"%s", msg.c_str()];
    } else {
        NSLog(@"[%s] %s", EventSeverityClass(severity).c_str(), msg.c_str());
    }
}

}
