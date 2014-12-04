#ifndef DISABLE_STOPWATCH
#include <mbgl/util/stopwatch.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>

#include <iostream>
#include <atomic>

using namespace mbgl::util;

stopwatch::stopwatch(Event event_)
    : event(event_), start(now()) {}

stopwatch::stopwatch(EventSeverity severity_, Event event_)
    : severity(severity_), event(event_), start(now()) {}

stopwatch::stopwatch(const std::string &name_, Event event_)
    : name(name_), event(event_), start(now()) {}

stopwatch::stopwatch(const std::string &name_, EventSeverity severity_, Event event_)
    : name(name_), severity(severity_), event(event_), start(now()) {}

void stopwatch::report(const std::string &name_) {
    timestamp duration = now() - start;

    Log::Record(severity, event, name_ + ": " + util::toString(double(duration) / 1_millisecond) + "ms");
    start += duration;
}

stopwatch::~stopwatch() {
    if (name.size()) {
        report(name);
    }
}
#endif
