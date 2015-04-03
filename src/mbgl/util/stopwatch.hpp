#ifndef MBGL_UTIL_STOPWATCH
#define MBGL_UTIL_STOPWATCH

#include <mbgl/platform/event.hpp>
#include <mbgl/util/chrono.hpp>

#include <string>

namespace mbgl {
namespace util {

#ifndef DISABLE_STOPWATCH
class stopwatch {
public:
    stopwatch(Event event = Event::General);
    stopwatch(EventSeverity severity, Event event = Event::General);
    stopwatch(const std::string &name, Event event = Event::General);
    stopwatch(const std::string &name, EventSeverity severity, Event event = Event::General);
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
    inline stopwatch(Event event = Event::General);
    inline stopwatch(EventSeverity severity, Event event = Event::General);
    inline stopwatch(const std::string &name, Event event = Event::General);
    inline stopwatch(const std::string &name, EventSeverity severity, Event event = Event::General);
    inline void report(const std::string &name) {}
    inline ~stopwatch() {}
};
#endif
}
}

#endif
