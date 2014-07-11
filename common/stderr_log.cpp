#include "stderr_log.hpp"

#include <iostream>

namespace llmr {

void StderrLogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    std::cerr << "[" << severity << "] " << event << ": " << msg << std::endl;
}

void StderrLogBackend::record(EventSeverity severity, Event event, int64_t code) {
    std::cerr << "[" << severity << "] " << event << ": (" << code << ")" << std::endl;
}

void StderrLogBackend::record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    std::cerr << "[" << severity << "] " << event << ": (" << code << ") " << msg << std::endl;

}

}
