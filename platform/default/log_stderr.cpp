#include <mbgl/platform/log.hpp>

#include <iostream>

namespace mbgl {

void Log::platformRecord(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    std::cerr << "[" << severity << "] " << event << ": (" << code << ") " << msg << std::endl;
}

}
