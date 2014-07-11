#ifndef LLMR_PLATFORM_EVENT
#define LLMR_PLATFORM_EVENT

#include <cstdint>
#include <ostream>

namespace llmr {

enum class EventSeverity : uint8_t {
    Debug,
    Info,
    Test,
    Warning,
    Error,
};

EventSeverity parseEventSeverity(const char *name);
const char *stringifyEventSeverity(EventSeverity eventSeverity);
::std::ostream& operator<<(::std::ostream& os, EventSeverity eventSeverity);

enum class Event : uint8_t {
    General,
    Setup,
    Shader,
    ParseStyle,
    ParseTile,
    Render,
    HttpRequest,
};

Event parseEvent(const char *name);
const char *stringifyEvent(Event event);
::std::ostream& operator<<(::std::ostream& os, Event event);

struct EventPermutation {
    const EventSeverity severity;
    const Event event;

    constexpr bool operator==(const EventPermutation &rhs) const {
        return severity == rhs.severity && event == rhs.event;
    }
};

constexpr EventSeverity disabledEventSeverities[] = {
#if !DEBUG
    EventSeverity::Debug,
#endif
#if !TESTING
    EventSeverity::Test,
#endif
};


constexpr Event disabledEvents[] = {
};

constexpr EventPermutation disabledEventPermutations[] = {
    { EventSeverity::Debug, Event::Shader }
};

}

#endif
