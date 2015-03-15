#include <mbgl/platform/log.hpp>

#include <iostream>

namespace mbgl {

void Log::platformRecord(EventSeverity severity, const std::string &msg) {
    std::cerr << "[" << severity << "] " << msg << std::endl;
}

}
