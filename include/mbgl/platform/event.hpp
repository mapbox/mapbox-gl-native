#ifndef MBGL_PLATFORM_EVENT
#define MBGL_PLATFORM_EVENT

#include <mbgl/util/enum.hpp>

#include <cstdint>

namespace mbgl {

enum class EventSeverity : uint8_t {
    Debug,
    Info,
    Warning,
    Error,
};

MBGL_DEFINE_ENUM_CLASS(EventSeverityClass, EventSeverity, {
    { EventSeverity::Debug, "DEBUG" },
    { EventSeverity::Info, "INFO" },
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
    Style,
    Database,
    HttpRequest,
    Sprite,
    Image,
    OpenGL,
    JNI,
    Android,
    Crash
};

MBGL_DEFINE_ENUM_CLASS(EventClass, Event, {
    { Event::General, "General" },
    { Event::Setup, "Setup" },
    { Event::Shader, "Shader" },
    { Event::ParseStyle, "ParseStyle" },
    { Event::ParseTile, "ParseTile" },
    { Event::Render, "Render" },
    { Event::Style, "Style" },
    { Event::Database, "Database" },
    { Event::HttpRequest, "HttpRequest" },
    { Event::Sprite, "Sprite" },
    { Event::Image, "Image" },
    { Event::OpenGL, "OpenGL" },
    { Event::JNI, "JNI" },
    { Event::Android, "Android" },
    { Event::Crash, "Crash" },
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
#if DEBUG
    EventSeverity(-1) // Avoid zero size array
#else
    EventSeverity::Debug
#endif
};

constexpr Event disabledEvents[] = {
    Event(-1) // Avoid zero size array
};

constexpr EventPermutation disabledEventPermutations[] = {
    { EventSeverity::Debug, Event::Shader }
};

}

#endif
