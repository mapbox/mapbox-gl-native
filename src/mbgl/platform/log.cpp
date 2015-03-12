#include <mbgl/platform/log.hpp>

#include <cstdarg>

namespace mbgl {

namespace {

static std::unique_ptr<Log::Observer> currentObserver;

}

void Log::setObserver(std::unique_ptr<Observer> observer) {
    currentObserver = std::move(observer);
}

void Log::record(EventSeverity severity, Event event, const std::string &msg) {
    record(severity, event, -1, msg);
}

void Log::record(EventSeverity severity, Event event, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char msg[4096];
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    record(severity, event, -1, msg);
}

void Log::record(EventSeverity severity, Event event, int64_t code) {
    record(severity, event, code, std::string());
}

void Log::record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    if (currentObserver && currentObserver->onRecord(severity, event, code, msg)) {
        return;
    }

    platformRecord(severity, event, code, msg);
}

}
