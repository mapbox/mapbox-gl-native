#ifndef LLMR_PLATFORM_EVENT
#define LLMR_PLATFORM_EVENT

#include <llmr/util/enum.hpp>

#include <cstdint>

namespace llmr {

enum class EventSeverity : uint8_t {
    Debug,
    Info,
    Test,
    Warning,
    Error,
};

LLMR_DEFINE_ENUM_CLASS(EventSeverityClass, EventSeverity, {
    { EventSeverity::Debug, "DEBUG" },
    { EventSeverity::Info, "INFO" },
    { EventSeverity::Test, "TEST" },
    { EventSeverity::Warning, "WARNING" },
    { EventSeverity::Error, "ERROR" },
    { EventSeverity(-1), "UNKNOWN" },
});

enum class Event : uint8_t {
    General,
    Setup,
    Shader,
    ParseStyle,
    ParseTile,
    Render,
    HttpRequest,
    Sprite,
};

LLMR_DEFINE_ENUM_CLASS(EventClass, Event, {
    { Event::General, "General" },
    { Event::Setup, "Setup" },
    { Event::Shader, "Shader" },
    { Event::ParseStyle, "ParseStyle" },
    { Event::ParseTile, "ParseTile" },
    { Event::Render, "Render" },
    { Event::HttpRequest, "HttpRequest" },
    { Event::Sprite, "Sprite" },
    { Event(-1), "Unknown" },
});


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
