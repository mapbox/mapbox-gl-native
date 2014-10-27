#ifndef DISABLE_TIMER
#include <mbgl/util/timer.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/platform/log.hpp>

#include <iostream>
#include <atomic>

using namespace mbgl::util;

timer::timer(Event event_)
    : event(event_), start(now()) {}

timer::timer(EventSeverity severity_, Event event_)
    : severity(severity_), event(event_), start(now()) {}

timer::timer(const std::string &name_, Event event_)
    : name(name_), event(event_), start(now()) {}

timer::timer(const std::string &name_, EventSeverity severity_, Event event_)
    : name(name_), severity(severity_), event(event_), start(now()) {}

void timer::report(const std::string &name_) {
    timestamp duration = now() - start;

    Log::Record(severity, event, name_ + ": " + std::to_string((double)(duration) / 1_millisecond) + "ms");
    start += duration;
}

timer::~timer() {
    if (name.size()) {
        report(name);
    }
}
#endif
