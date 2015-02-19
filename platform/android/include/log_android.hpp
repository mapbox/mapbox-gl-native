#ifndef MBGL_PLATFORM_ANDROID_LOG_ANDROID
#define MBGL_PLATFORM_ANDROID_LOG_ANDROID

#include <mbgl/platform/log.hpp>

namespace mbgl {

class AndroidLogBackend : public LogBackend {
private:
    int severityToPriority(EventSeverity severity);

public:
    inline ~AndroidLogBackend() = default;

    void record(EventSeverity severity, Event event, const std::string &msg);
    void record(EventSeverity severity, Event event, const char* format, ...);
    void record(EventSeverity severity, Event event, int64_t code);
    void record(EventSeverity severity, Event event, int64_t code, const std::string &msg);
};


}

#endif
