#include "fixture_log.hpp"

namespace mbgl {

FixtureLogBackend::LogMessage::LogMessage(EventSeverity severity_, Event event_, int64_t code_,
                                          const std::string &msg_)
    : severity(severity_), event(event_), code(code_), msg(msg_) {
}
FixtureLogBackend::LogMessage::LogMessage(EventSeverity severity_, Event event_, int64_t code_)
    : severity(severity_), event(event_), code(code_), msg() {
}
FixtureLogBackend::LogMessage::LogMessage(EventSeverity severity_, Event event_,
                                          const std::string &msg_)
    : severity(severity_), event(event_), code(), msg(msg_) {
}

FixtureLogBackend::LogMessage::LogMessage(EventSeverity severity_, Event event_)
    : severity(severity_), event(event_), code(), msg() {
}

bool FixtureLogBackend::LogMessage::operator==(const LogMessage &rhs) const {
    return (!severity || !rhs.severity || severity.get() == rhs.severity.get()) &&
           (!event || !rhs.event || event.get() == rhs.event.get()) &&
           (!code || !rhs.code || code.get() == rhs.code.get()) &&
           (!msg || !rhs.msg || msg.get() == rhs.msg.get());
}

FixtureLogBackend::~FixtureLogBackend() {
    std::cerr << unchecked();
}

void FixtureLogBackend::record(EventSeverity severity, Event event, const std::string &msg) {
    messages.emplace_back(severity, event, msg);
}

void FixtureLogBackend::record(EventSeverity severity, Event event, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const size_t len = vsnprintf(NULL, 0, format, args);
    va_end(args);
    std::unique_ptr<char[]> buffer(new char[len + 1]);
    va_start(args, format);
    vsnprintf(buffer.get(), len + 1, format, args);
    va_end(args);
    messages.emplace_back(severity, event, std::string{ buffer.get(), len });
}

void FixtureLogBackend::record(EventSeverity severity, Event event, int64_t code) {
    messages.emplace_back(severity, event, code);
}

void FixtureLogBackend::record(EventSeverity severity, Event event, int64_t code,
                               const std::string &msg) {
    messages.emplace_back(severity, event, code, msg);
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

::std::ostream &operator<<(::std::ostream &os,
                           const std::vector<FixtureLogBackend::LogMessage> &messages) {
    for (const FixtureLogBackend::LogMessage &message : messages) {
        os << "- " << message;
    }
    return os;
}

::std::ostream &operator<<(::std::ostream &os, const FixtureLogBackend::LogMessage &message) {
    os << "[\"" << message.severity.get() << "\", \"" << message.event.get() << "\"";
    if (message.code)
        os << ", " << message.code.get();
    if (message.msg)
        os << ", \"" << message.msg.get() << "\"";
    return os << "]" << std::endl;
}

}