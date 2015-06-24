#include <mbgl/platform/log.hpp>

#include <mbgl/util/thread_context.hpp>

#include <cstdarg>
#include <sstream>

namespace mbgl {

namespace {

static std::unique_ptr<Log::Observer> currentObserver;

}

void Log::setObserver(std::unique_ptr<Observer> observer) {
    currentObserver = std::move(observer);
}

std::unique_ptr<Log::Observer> Log::removeObserver() {
    std::unique_ptr<Observer> observer;
    std::swap(observer, currentObserver);
    return observer;
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
    if (currentObserver && severity != EventSeverity::Debug &&
        currentObserver->onRecord(severity, event, code, msg)) {
        return;
    }

    std::stringstream logStream;
    logStream << "{" << util::ThreadContext::getName() << "}";
    logStream << "[" << event << "]";

    if (code >= 0) {
        logStream << "(" << code << ")";
    }

    if (!msg.empty()) {
        logStream << ": " << msg;
    }

    platformRecord(severity, logStream.str());
}

}
