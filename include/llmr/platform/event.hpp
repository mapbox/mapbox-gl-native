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
::std::ostream& operator<<(::std::ostream& os, EventSeverity eventSeverity);

enum class Event : uint8_t {
    ParseStyle,
    ParseTile,
    Render,
    HttpRequest,
};

Event parseEvent(const char *name);
::std::ostream& operator<<(::std::ostream& os, Event event);

constexpr EventSeverity enabledEventSeverities[] = {
#if DEBUG
    EventSeverity::Debug,
#endif

#if TESTING
    EventSeverity::Test,
#endif

    EventSeverity::Info,
    EventSeverity::Warning,
    EventSeverity::Error,
};

/* enabled event classes */
constexpr Event enabledEvents[] = {
    Event::ParseStyle,
    Event::ParseTile,
    Event::HttpRequest,
};

}

#endif
