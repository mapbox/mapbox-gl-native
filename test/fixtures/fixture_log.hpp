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
        inline LogMessage(EventSeverity severity_, Event event_, int64_t code_, const std::string &msg_)
            : severity(severity_), event(event_), code(code_), msg(msg_) {}
        inline LogMessage(EventSeverity severity_, Event event_, int64_t code_)
            : severity(severity_), event(event_), code(code_), msg() {}
        inline LogMessage(EventSeverity severity_, Event event_, const std::string &msg_)
            : severity(severity_), event(event_), code(), msg(msg_) {}
        inline LogMessage(EventSeverity severity_, Event event_)
            : severity(severity_), event(event_), code(), msg() {}

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

    inline ~FixtureLogBackend();

    inline void record(EventSeverity severity, Event event, const std::string &msg) {
        messages.emplace_back(severity, event, msg);
    }

    inline void record(EventSeverity severity, Event event, const char* format, ...) {
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

    inline void record(EventSeverity severity, Event event, int64_t code) {
        messages.emplace_back(severity, event, code);
    }

    inline void record(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
        messages.emplace_back(severity, event, code, msg);
    }

    inline size_t count(const LogMessage &message) const;
    inline std::vector<LogMessage> unchecked() const;

public:
    std::vector<LogMessage> messages;
};

inline ::std::ostream& operator<<(::std::ostream& os, const std::vector<FixtureLogBackend::LogMessage>& messages);
inline ::std::ostream& operator<<(::std::ostream& os, const FixtureLogBackend::LogMessage& message);

FixtureLogBackend::~FixtureLogBackend() {
    std::cerr << unchecked();
}

size_t FixtureLogBackend::count(const LogMessage &message) const {
    size_t message_count = 0;
    for (const LogMessage &msg : messages) {
        if (msg == message) {
            message_count++;
            msg.checked = true;
        }
    }
    return message_count;
}

std::vector<FixtureLogBackend::LogMessage> FixtureLogBackend::unchecked() const {
    std::vector<LogMessage> unchecked_messages;
    for (const LogMessage &msg : messages) {
        if (!msg.checked) {
            unchecked_messages.push_back(msg);
            msg.checked = true;
        }
    }
    return unchecked_messages;
}

::std::ostream& operator<<(::std::ostream& os, const std::vector<FixtureLogBackend::LogMessage>& messages) {
    for (const FixtureLogBackend::LogMessage &message : messages) {
        os << "- " << message;
    }
    return os;
}

::std::ostream& operator<<(::std::ostream& os, const FixtureLogBackend::LogMessage& message) {
    os << "[\"" << message.severity.get() << "\", \"" << message.event.get() << "\"";
    if (message.code) os << ", " << message.code.get();
    if (message.msg) os << ", \"" << message.msg.get() << "\"";
    return os << "]" << std::endl;
}

}

#endif
