#include <llmr/platform/event.hpp>

#include <cstring>

namespace llmr {

EventSeverity parseEventSeverity(const char *name) {
    if (strcmp(name, "DEBUG") == 0) return EventSeverity::Debug;
    if (strcmp(name, "INFO") == 0) return EventSeverity::Info;
    if (strcmp(name, "TEST") == 0) return EventSeverity::Test;
    if (strcmp(name, "WARNING") == 0) return EventSeverity::Warning;
    if (strcmp(name, "ERROR") == 0) return EventSeverity::Error;
    return EventSeverity(-1);
}

const char *stringifyEventSeverity(EventSeverity eventSeverity) {
    switch (eventSeverity) {
        case EventSeverity::Debug: return "DEBUG";
        case EventSeverity::Info: return "INFO";
        case EventSeverity::Test: return "TEST";
        case EventSeverity::Warning: return "WARNING";
        case EventSeverity::Error: return "ERROR";
        default: return "UNKNOWN";
    }
}

::std::ostream& operator<<(::std::ostream& os, EventSeverity eventSeverity) {
    return os << stringifyEventSeverity(eventSeverity);
}

Event parseEvent(const char *name) {
    if (strcmp(name, "General") == 0) return Event::General;
    if (strcmp(name, "Setup") == 0) return Event::Setup;
    if (strcmp(name, "Shader") == 0) return Event::Shader;
    if (strcmp(name, "ParseStyle") == 0) return Event::ParseStyle;
    if (strcmp(name, "ParseTile") == 0) return Event::ParseTile;
    if (strcmp(name, "Render") == 0) return Event::Render;
    if (strcmp(name, "HttpRequest") == 0) return Event::HttpRequest;
    return Event(-1);
}

const char *stringifyEvent(Event event) {
     switch (event) {
        case Event::General: return "General";
        case Event::Setup: return "Setup";
        case Event::Shader: return "Shader";
        case Event::ParseStyle: return "ParseStyle";
        case Event::ParseTile: return "ParseTile";
        case Event::Render: return "Render";
        case Event::HttpRequest: return "HttpRequest";
        default: return "Unknown";
    }
}

::std::ostream& operator<<(::std::ostream& os, Event event) {
    return os << stringifyEvent(event);
}


}
