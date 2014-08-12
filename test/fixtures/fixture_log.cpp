#include "fixture_log.hpp"

#include <iostream>

namespace mbgl {

FixtureLogBackend::~FixtureLogBackend() {
    std::cerr << unchecked();
}

size_t FixtureLogBackend::count(const LogMessage &message) const {
    size_t count = 0;
    for (const LogMessage &msg : messages) {
        if (msg == message) {
            count++;
            msg.checked = true;
        }
    }
    return count;
}

std::vector<FixtureLogBackend::LogMessage> FixtureLogBackend::unchecked() const {
    std::vector<LogMessage> unchecked;
    for (const LogMessage &msg : messages) {
        if (!msg.checked) {
            unchecked.push_back(msg);
            msg.checked = true;
        }
    }
    return unchecked;
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
