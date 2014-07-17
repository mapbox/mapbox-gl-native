#ifndef MBGL_COMMON_STDERR_LOG
#define MBGL_COMMON_STDERR_LOG

#include <mbgl/platform/log.hpp>

namespace mbgl {

class StderrLogBackend : public LogBackend {
public:
    inline ~StderrLogBackend() = default;

    void record(EventSeverity severity, Event event, const std::string &msg);
    void record(EventSeverity severity, Event event, const char* format, ...);
    void record(EventSeverity severity, Event event, int64_t code);
    void record(EventSeverity severity, Event event, int64_t code, const std::string &msg);
};


}

#endif
