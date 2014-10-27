#include "fixture_log.hpp"

#include <iostream>

namespace mbgl {

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
