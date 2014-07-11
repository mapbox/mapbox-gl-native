#include "stderr_log.hpp"

#include <iostream>

namespace llmr {

void StderrLogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    std::cerr << "[" << severity << "] " << event << ": " << msg << std::endl;
}

void StderrLogBackend::record(EventSeverity severity, Event event, const char* format, ...) {
    std::cerr << "[" << severity << "] " << event << ": ";
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    std::cerr << std::endl;
}

void StderrLogBackend::record(EventSeverity severity, Event event, int64_t code) {
    std::cerr << "[" << severity << "] " << event << ": (" << code << ")" << std::endl;
}

void StderrLogBackend::record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    std::cerr << "[" << severity << "] " << event << ": (" << code << ") " << msg << std::endl;

}

}
