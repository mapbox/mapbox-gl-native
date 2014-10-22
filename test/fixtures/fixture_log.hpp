#ifndef MBGL_TEST_FIXTURE_LOG
#define MBGL_TEST_FIXTURE_LOG

#include <mbgl/platform/log.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <cstdarg>

namespace mbgl {

class FixtureLogBackend : public LogBackend, private util::noncopyable {
public:
    struct LogMessage {
        inline LogMessage(EventSeverity severity, Event event, int64_t code, const std::string &msg)
            : severity(severity), event(event), code(code), msg(msg) {}
        inline LogMessage(EventSeverity severity, Event event, int64_t code)
            : severity(severity), event(event), code(code), msg() {}
        inline LogMessage(EventSeverity severity, Event event, const std::string &msg)
            : severity(severity), event(event), code(), msg(msg) {}
        inline LogMessage(EventSeverity severity, Event event)
            : severity(severity), event(event), code(), msg() {}

        inline bool operator==(const LogMessage &rhs) const {
            return (!severity || !rhs.severity || severity.get() == rhs.severity.get()) &&
                   (!event || !rhs.event || event.get() == rhs.event.get()) &&
                   (!code || !rhs.code || code.get() == rhs.code.get()) &&
                   (!msg || !rhs.msg || msg.get() == rhs.msg.get());
        }

        const mapbox::util::optional<EventSeverity> severity;
        const mapbox::util::optional<Event> event;
        const mapbox::util::optional<int64_t> code;
        const mapbox::util::optional<std::string> msg;

        mutable bool checked = false;
    };

    ~FixtureLogBackend();

    void record(EventSeverity severity, Event event, const std::string &msg) {
        messages.emplace_back(severity, event, msg);
    }

    void record(EventSeverity severity, Event event, const char* format, ...) {
        va_list args;
        va_start(args, format);
        const size_t len = vsnprintf(NULL, 0, format, args);
        va_end(args);
        std::unique_ptr<char[]> buffer(new char[len + 1]);
        va_start(args, format);
        vsnprintf(buffer.get(), len + 1, format, args);
        va_end(args);
        messages.emplace_back(severity, event, std::string { buffer.get(), len });
    }

    void record(EventSeverity severity, Event event, int64_t code) {
        messages.emplace_back(severity, event, code);
    }

    void record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
        messages.emplace_back(severity, event, code, msg);
    }

    size_t count(const LogMessage &message) const;
    std::vector<LogMessage> unchecked() const;

public:
    std::vector<LogMessage> messages;
};

::std::ostream& operator<<(::std::ostream& os, const std::vector<FixtureLogBackend::LogMessage>& messages);
::std::ostream& operator<<(::std::ostream& os, const FixtureLogBackend::LogMessage& message);


}

#endif
