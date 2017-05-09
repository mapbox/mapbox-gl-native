#include <mbgl/test/fixture_log_observer.hpp>
#include <mbgl/test/util.hpp>
#include <mbgl/util/enum.hpp>

namespace mbgl {

FixtureLog::Message::Message(EventSeverity severity_,
                             Event event_,
                             int64_t code_,
                             std::string msg_)
    : severity(severity_), event(event_), code(code_), msg(std::move(msg_)) {
}

bool FixtureLog::Message::operator==(const Message& rhs) const {
    return severity == rhs.severity && event == rhs.event && code == rhs.code && msg == rhs.msg;
}

FixtureLog::Observer::Observer(FixtureLog* log_) : log(log_) {
}

FixtureLog::Observer::~Observer() {
    if (log) {
        log->observer = nullptr;
    }
    std::cerr << unchecked();
}

bool FixtureLog::Observer::onRecord(EventSeverity severity,
                                    Event event,
                                    int64_t code,
                                    const std::string& msg) {
    std::lock_guard<std::mutex> lock(messagesMutex);

    messages.emplace_back(severity, event, code, msg);

    return true;
}

bool FixtureLog::Observer::empty() const {
    std::lock_guard<std::mutex> lock(messagesMutex);

    return messages.empty();
}

size_t FixtureLog::Observer::count(const Message& message) const {
    std::lock_guard<std::mutex> lock(messagesMutex);

    size_t message_count = 0;
    for (const auto& msg : messages) {
        if (msg == message) {
            message_count++;
            msg.checked = true;
        }
    }
    return message_count;
}

FixtureLog::FixtureLog() : observer(new FixtureLogObserver(this)) {
    Log::setObserver(std::unique_ptr<Log::Observer>(observer));
}

bool FixtureLog::empty() const {
    return observer ? observer->empty() : true;
}

size_t FixtureLog::count(const FixtureLog::Message& message) const {
    return observer ? observer->count(message) : 0;
}

FixtureLog::~FixtureLog() {
    if (observer) {
        Log::removeObserver();
    }
}

std::vector<FixtureLog::Message> FixtureLogObserver::unchecked() const {
    std::lock_guard<std::mutex> lock(messagesMutex);

    std::vector<Message> unchecked_messages;
    for (const auto& msg : messages) {
        if (!msg.checked) {
            unchecked_messages.push_back(msg);
            msg.checked = true;
        }
    }
    return unchecked_messages;
}

::std::ostream& operator<<(::std::ostream& os, const std::vector<FixtureLog::Message>& messages) {
    for (const auto& message : messages) {
        os << "- " << message;
    }
    return os;
}

::std::ostream& operator<<(::std::ostream& os, const FixtureLog::Message& message) {
    os << R"([")" << Enum<EventSeverity>::toString(message.severity) << R"(", ")";
    os << Enum<Event>::toString(message.event) << R"(")";
    os << ", " << message.code;
    os << R"(, ")" << message.msg << R"(")";
    return os << "]" << std::endl;
}

} // namespace mbgl
