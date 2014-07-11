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

::std::ostream& operator<<(::std::ostream& os, EventSeverity eventSeverity) {
    switch (eventSeverity) {
        case EventSeverity::Debug: return os << "DEBUG";
        case EventSeverity::Info: return os << "INFO";
        case EventSeverity::Test: return os << "TEST";
        case EventSeverity::Warning: return os << "WARNING";
        case EventSeverity::Error: return os << "ERROR";
        default: return os << "UNKNOWN";
    }
}

Event parseEvent(const char *name) {
    if (strcmp(name, "ParseStyle") == 0) return Event::ParseStyle;
    if (strcmp(name, "ParseTile") == 0) return Event::ParseTile;
    if (strcmp(name, "Render") == 0) return Event::Render;
    if (strcmp(name, "HttpRequest") == 0) return Event::HttpRequest;
    return Event(-1);
}

::std::ostream& operator<<(::std::ostream& os, Event event) {
    switch (event) {
        case Event::ParseStyle: return os << "ParseStyle";
        case Event::ParseTile: return os << "ParseTile";
        case Event::Render: return os << "Render";
        case Event::HttpRequest: return os << "HttpRequest";
        default: return os << "Unknown";
    }
}


}
