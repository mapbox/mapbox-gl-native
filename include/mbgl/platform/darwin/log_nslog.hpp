#ifndef MBGL_COMMON_NSLOG_LOG
#define MBGL_COMMON_NSLOG_LOG

#include <mbgl/platform/log.hpp>

namespace mbgl {

class NSLogBackend : public LogBackend {
public:
    inline ~NSLogBackend() = default;

    void record(EventSeverity severity, Event event, int64_t code, const std::string &msg);
};


}

#endif
