#ifndef DISABLE_STOPWATCH
#include <mbgl/util/stopwatch.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>

#include <iostream>
#include <atomic>

using namespace mbgl::util;

stopwatch::stopwatch(Event event_)
    : event(event_), start(std::chrono::steady_clock::now()) {}

stopwatch::stopwatch(EventSeverity severity_, Event event_)
    : severity(severity_), event(event_), start(std::chrono::steady_clock::now()) {}

stopwatch::stopwatch(const std::string &name_, Event event_)
    : name(name_), event(event_), start(std::chrono::steady_clock::now()) {}

stopwatch::stopwatch(const std::string &name_, EventSeverity severity_, Event event_)
    : name(name_), severity(severity_), event(event_), start(std::chrono::steady_clock::now()) {}

void stopwatch::report(const std::string &name_) {
    std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - start;

    Log::Record(severity, event, name_ + ": " + util::toString(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + "ms");
    start += duration;
}

stopwatch::~stopwatch() {
    if (name.size()) {
        report(name);
    }
}
#endif
