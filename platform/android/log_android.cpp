#include <mbgl/platform/android/log_android.hpp>

#define __STDC_FORMAT_MACROS // NDK bug workaround: https://code.google.com/p/android/issues/detail?id=72349
#include <cinttypes>

#include <android/log.h>

namespace mbgl {

int AndroidLogBackend::severityToPriority(EventSeverity severity) {
    switch(severity) {
    case EventSeverity::Debug:
        return ANDROID_LOG_DEBUG;

    case EventSeverity::Info:
        return ANDROID_LOG_INFO;

    case EventSeverity::Warning:
        return ANDROID_LOG_WARN;

    case EventSeverity::Error:
        return ANDROID_LOG_ERROR;

    default:
        return ANDROID_LOG_VERBOSE;
    }
}

void AndroidLogBackend::record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    __android_log_print(severityToPriority(severity), EventClass(event).c_str(), "(%" PRId64 ") %s", code, msg.c_str());
}

}
