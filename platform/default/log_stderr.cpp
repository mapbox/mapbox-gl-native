#include <mbgl/platform/default/log_stderr.hpp>

#include <iostream>

namespace mbgl {

void StderrLogBackend::record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    std::cerr << "[" << severity << "] " << event << ": (" << code << ") " << msg << std::endl;
}

}
