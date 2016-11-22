#include <mbgl/util/logging.hpp>
#include <mbgl/util/enum.hpp>

#include <iostream>

namespace mbgl {

void Log::platformRecord(EventSeverity severity, const std::string &msg) {
    std::cerr << "[" << Enum<EventSeverity>::toString(severity) << "] " << msg << std::endl;
}

} // namespace mbgl
