#ifndef LLMR_PLATFORM_LOG
#define LLMR_PLATFORM_LOG

#include "event.hpp"

#include <memory>
#include <string>

namespace llmr {

class LogBackend {
public:
    virtual inline ~LogBackend() = default;
    virtual void record(EventSeverity severity, Event event, const std::string &msg) = 0;
    virtual void record(EventSeverity severity, Event event, int64_t code) = 0;
    virtual void record(EventSeverity severity, Event event, int64_t code, const std::string &msg) = 0;
};

class Log {
private:
    template <typename T>
    constexpr static bool includes(const T e, T const *l, const size_t i = 0) {
        return i >= sizeof l ? false : *(l + i) == e ? true : includes(e, l, i + 1);
    }

public:
    template <typename ...Args>
    static inline void Debug(Event event, Args&& ...args) {
        Record(EventSeverity::Debug, event, ::std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline void Info(Event event, Args&& ...args) {
        Record(EventSeverity::Info, event, ::std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline void Warning(Event event, Args&& ...args) {
        Record(EventSeverity::Warning, event, ::std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline void Error(Event event, Args&& ...args) {
        Record(EventSeverity::Error, event, ::std::forward<Args>(args)...);
    }

    template <typename ...Args>
    static inline void Record(EventSeverity severity, Event event, Args&& ...args) {
        if (includes(severity, enabledEventSeverities) &&
            includes(event, enabledEvents)) {
            if (Backend) {
                Backend->record(severity, event, ::std::forward<Args>(args)...);
            }
        }
    }

    template<typename T, typename ...Args>
    static inline const T &Set(Args&& ...args) {
        Backend = ::std::unique_ptr<T>(new T(::std::forward<Args>(args)...));
        return *dynamic_cast<T *>(Backend.get());
    }

private:
    static std::unique_ptr<LogBackend> Backend;
};

}

#endif
