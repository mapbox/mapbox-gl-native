#pragma once

#include <mbgl/util/event.hpp>
#include <mbgl/util/chrono.hpp>

#include <string>

namespace mbgl {
namespace util {

#ifndef DISABLE_STOPWATCH
class stopwatch {
public:
    stopwatch(Event event = Event::General);
    stopwatch(EventSeverity severity, Event event = Event::General);
    stopwatch(std::string name, Event event = Event::General);
    stopwatch(std::string name, EventSeverity severity, Event event = Event::General);
    void report(const std::string &name);
    ~stopwatch();

private:
    const std::string name;
    EventSeverity severity = EventSeverity::Debug;
    Event event = Event::General;
    TimePoint start;
};
#else
class stopwatch {
    stopwatch(Event event = Event::General);
    stopwatch(EventSeverity severity, Event event = Event::General);
    stopwatch(const std::string &name, Event event = Event::General);
    stopwatch(const std::string &name, EventSeverity severity, Event event = Event::General);
    void report(const std::string &name) {}
    ~stopwatch() {}
};
#endif
} // namespace util
} // namespace mbgl
