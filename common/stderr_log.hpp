#ifndef LLMR_COMMON_STDERR_LOG
#define LLMR_COMMON_STDERR_LOG

#include <llmr/platform/log.hpp>

namespace llmr {

class StderrLogBackend : public LogBackend {
public:
    inline ~StderrLogBackend() = default;

    void record(EventSeverity severity, Event event, const std::string &msg);
    void record(EventSeverity severity, Event event, const char* format, ...);
    void record(EventSeverity severity, Event event, int64_t code);
    void record(EventSeverity severity, Event event, int64_t code, const std::string &msg);
};


}

#endif
