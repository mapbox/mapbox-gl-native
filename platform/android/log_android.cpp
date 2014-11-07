#include <mbgl/platform/android/log_android.hpp>

#include <iostream>
#include <cstdarg>
#define __STDC_FORMAT_MACROS // NDK bug workaround: https://code.google.com/p/android/issues/detail?id=72349
#include <cinttypes>

#include <android/log.h>

namespace mbgl {

int AndroidLogBackend::severity_to_priority(EventSeverity severity) {
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

void AndroidLogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    __android_log_print(severity_to_priority(severity), EventClass(event).c_str(), "%s", msg.c_str());
}

void AndroidLogBackend::record(EventSeverity severity, Event event, const char* format, ...) {
    va_list args;
    va_start(args, format);

    const int len = vsnprintf(nullptr, 0, format, args) + 1;
    char* buf = new char[len];
    vsnprintf(buf, len, format, args);

    va_end(args);

    __android_log_print(severity_to_priority(severity), EventClass(event).c_str(), "%s", buf);

    delete buf;
    buf  = nullptr;
}

void AndroidLogBackend::record(EventSeverity severity, Event event, int64_t code) {
    __android_log_print(severity_to_priority(severity), EventClass(event).c_str(), "(%" PRId64 ")", code);
}

void AndroidLogBackend::record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    __android_log_print(severity_to_priority(severity), EventClass(event).c_str(), "(%" PRId64 ") %s", code, msg.c_str());
}

}
