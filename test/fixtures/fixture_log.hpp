#ifndef LLMR_TEST_FIXTURE_LOG
#define LLMR_TEST_FIXTURE_LOG

#include <llmr/platform/log.hpp>

#include <boost/optional.hpp>

#include <vector>

namespace llmr {

class FixtureLogBackend : public LogBackend {
public:
    struct LogMessage {
        inline LogMessage(EventSeverity severity, Event event, int64_t code, const std::string &msg)
            : severity(severity), event(event), code(code), msg(msg) {}
        inline LogMessage(EventSeverity severity, Event event, int64_t code)
            : severity(severity), event(event), code(code) {}
        inline LogMessage(EventSeverity severity, Event event, const std::string &msg)
            : severity(severity), event(event), msg(msg) {}
        inline LogMessage(EventSeverity severity, Event event)
            : severity(severity), event(event) {}

        inline bool operator==(const LogMessage &rhs) const {
            return (!severity || !rhs.severity || severity.get() == rhs.severity.get()) &&
                   (!event || !rhs.event || event.get() == rhs.event.get()) &&
                   (!code || !rhs.code || code.get() == rhs.code.get()) &&
                   (!msg || !rhs.msg || msg.get() == rhs.msg.get());
        }

        const boost::optional<EventSeverity> severity;
        const boost::optional<Event> event;
        const boost::optional<int64_t> code;
        const boost::optional<std::string> msg;

        mutable bool checked = false;
    };

    inline ~FixtureLogBackend() = default;

    void record(EventSeverity severity, Event event, const std::string &msg) {
        messages.emplace_back(severity, event, msg);
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
