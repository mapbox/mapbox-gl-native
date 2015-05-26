#ifndef MBGL_PLATFORM_LOG
#define MBGL_PLATFORM_LOG

#include <mbgl/platform/event.hpp>

#include <mbgl/util/noncopyable.hpp>

#include <memory>
#include <string>

namespace mbgl {

class Log {
public:
    class Observer : private util::noncopyable {
    public:
        virtual ~Observer() = default;

        // When an observer is set, this function will be called for every log
        // message. Returning true will consume the message.
        virtual bool onRecord(EventSeverity severity, Event event, int64_t code, const std::string &msg) = 0;
    };

    static void setObserver(std::unique_ptr<Observer> Observer);
    static std::unique_ptr<Observer> removeObserver();

private:
    template <typename T, size_t N>
    constexpr static bool includes(const T e, const T (&l)[N], const size_t i = 0) {
        return i < N && (l[i] == e || includes(e, l, i + 1));
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
        if (!includes(severity, disabledEventSeverities) &&
            !includes(event, disabledEvents) &&
            !includes({ severity, event }, disabledEventPermutations)) {
                record(severity, event, ::std::forward<Args>(args)...);
        }
    }

private:
    static void record(EventSeverity severity, Event event, const std::string &msg);
    static void record(EventSeverity severity, Event event, const char* format, ...);
    static void record(EventSeverity severity, Event event, int64_t code);
    static void record(EventSeverity severity, Event event, int64_t code, const std::string &msg);

    // This method is the data sink that must be implemented by each platform we
    // support. It should ideally output the error message in a human readable
    // format to the developer.
    static void platformRecord(EventSeverity severity, const std::string &msg);
};

}

#endif
