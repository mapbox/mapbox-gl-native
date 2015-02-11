#ifndef MBGL_TEST_FIXTURE_LOG
#define MBGL_TEST_FIXTURE_LOG

#include <mbgl/platform/log.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <cstdarg>
#include <iostream>

namespace mbgl {

class FixtureLogBackend : public LogBackend, private util::noncopyable {
public:
    struct LogMessage {
        LogMessage(EventSeverity severity_, Event event_, int64_t code_, const std::string &msg_);
        LogMessage(EventSeverity severity_, Event event_, int64_t code_);
        LogMessage(EventSeverity severity_, Event event_, const std::string &msg_);
        LogMessage(EventSeverity severity_, Event event_);

        bool operator==(const LogMessage &rhs) const;

        const mapbox::util::optional<EventSeverity> severity;
        const mapbox::util::optional<Event> event;
        const mapbox::util::optional<int64_t> code;
        const mapbox::util::optional<std::string> msg;

        mutable bool checked = false;
    };

    ~FixtureLogBackend();

    inline void record(EventSeverity severity, Event event, const std::string &msg);
    inline void record(EventSeverity severity, Event event, const char *format, ...);
    inline void record(EventSeverity severity, Event event, int64_t code);
    inline void record(EventSeverity severity, Event event, int64_t code, const std::string &msg);

    size_t count(const LogMessage &message) const;
    std::vector<LogMessage> unchecked() const;

public:
    std::vector<LogMessage> messages;
};

::std::ostream &operator<<(::std::ostream &os,
                           const std::vector<FixtureLogBackend::LogMessage> &messages);
::std::ostream &operator<<(::std::ostream &os, const FixtureLogBackend::LogMessage &message);

}

#endif
