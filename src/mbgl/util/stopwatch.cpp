#ifndef DISABLE_STOPWATCH
#include <mbgl/util/stopwatch.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>

#include <iostream>
#include <atomic>

using namespace mbgl::util;

stopwatch::stopwatch(Event event_)
    : event(event_), start(Clock::now()) {}

stopwatch::stopwatch(EventSeverity severity_, Event event_)
    : severity(severity_), event(event_), start(Clock::now()) {}

stopwatch::stopwatch(const std::string &name_, Event event_)
    : name(name_), event(event_), start(Clock::now()) {}

stopwatch::stopwatch(const std::string &name_, EventSeverity severity_, Event event_)
    : name(name_), severity(severity_), event(event_), start(Clock::now()) {}

void stopwatch::report(const std::string &name_) {
    Duration duration = Clock::now() - start;

    Log::Record(severity, event, "%s: %fms", name_.c_str(), std::chrono::duration<float, std::chrono::milliseconds::period>(duration).count());
    start += duration;
}

stopwatch::~stopwatch() {
    if (!name.empty()) {
        report(name);
    }
}
#endif
