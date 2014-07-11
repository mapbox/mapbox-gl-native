#ifndef LLMR_UTIL_TIMER
#define LLMR_UTIL_TIMER

#include <llmr/platform/event.hpp>

#include <string>

namespace llmr {
namespace util {

#ifndef DISABLE_TIMER
class timer {
public:
    timer(Event event = Event::General);
    timer(EventSeverity severity, Event event = Event::General);
    timer(const std::string &name, Event event = Event::General);
    timer(const std::string &name, EventSeverity severity, Event event = Event::General);
    void report(const std::string &name);
    ~timer();

private:
    const std::string name;
    EventSeverity severity = EventSeverity::Debug;
    Event event = Event::General;
    uint64_t start;
};
#else
class timer {
    inline timer(Event event = Event::General);
    inline timer(EventSeverity severity, Event event = Event::General);
    inline timer(const std::string &name, Event event = Event::General);
    inline timer(const std::string &name, EventSeverity severity, Event event = Event::General);
    inline void report(const std::string &name) {}
    inline ~timer() {}
};
#endif
}
}

#endif
