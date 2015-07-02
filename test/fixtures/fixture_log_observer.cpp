#include "fixture_log_observer.hpp"
#include "../fixtures/util.hpp"

namespace mbgl {

FixtureLogObserver::LogMessage::LogMessage(EventSeverity severity_, Event event_, int64_t code_,
                                          const std::string &msg_)
    : severity(severity_), event(event_), code(code_), msg(msg_) {
}

bool FixtureLogObserver::LogMessage::operator==(const LogMessage &rhs) const {
    return severity == rhs.severity &&
           event == rhs.event &&
           code == rhs.code &&
           msg == rhs.msg;
}

FixtureLogObserver::~FixtureLogObserver() {
    std::cerr << unchecked();
}

bool FixtureLogObserver::onRecord(EventSeverity severity, Event event, int64_t code,
                                  const std::string &msg) {
    messages.emplace_back(severity, event, code, msg);

    return true;
}

size_t FixtureLogObserver::count(const LogMessage &message) const {
    size_t message_count = 0;
    for (const auto& msg : messages) {
        if (msg == message) {
            message_count++;
            msg.checked = true;
        }
    }
    return message_count;
}

std::vector<FixtureLogObserver::LogMessage> FixtureLogObserver::unchecked() const {
    std::vector<LogMessage> unchecked_messages;
    for (const auto& msg : messages) {
        if (!msg.checked) {
            unchecked_messages.push_back(msg);
            msg.checked = true;
        }
    }
    return unchecked_messages;
}

::std::ostream &operator<<(::std::ostream &os,
                           const std::vector<FixtureLogObserver::LogMessage> &messages) {
    for (const auto& message : messages) {
        os << "- " << message;
    }
    return os;
}

::std::ostream &operator<<(::std::ostream &os, const FixtureLogObserver::LogMessage &message) {
    os << "[\"" << message.severity << "\", \"" << message.event << "\"";
    os << ", " << message.code;
    os << ", \"" << message.msg << "\"";
    return os << "]" << std::endl;
}

}
