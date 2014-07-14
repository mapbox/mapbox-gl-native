#ifndef LLMR_COMMON_STDERR_LOG
#define LLMR_COMMON_STDERR_LOG

#include <llmr/platform/log.hpp>

namespace llmr {

class StderrLogBackend : public LogBackend {
public:
    inline ~StderrLogBackend() = default;

    void record(EventSeverityClass severity, EventClass event, const std::string &msg);
    void record(EventSeverityClass severity, EventClass event, const char* format, ...);
    void record(EventSeverityClass severity, EventClass event, int64_t code);
    void record(EventSeverityClass severity, EventClass event, int64_t code, const std::string &msg);
};


}

#endif
