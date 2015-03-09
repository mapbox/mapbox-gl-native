#include <mbgl/platform/log.hpp>

#include <cstdarg>

namespace mbgl {

std::unique_ptr<LogBackend> Log::Backend;

void LogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    record(severity, event, -1, msg);
}

void LogBackend::record(EventSeverity severity, Event event, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char msg[4096];
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    record(severity, event, -1, msg);
}

void LogBackend::record(EventSeverity severity, Event event, int64_t code) {
    record(severity, event, code, std::string());
}

}
