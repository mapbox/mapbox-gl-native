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

bool FixtureLog::Message::matches(const Message& rhs, bool substring) const {
    return severity == rhs.severity && event == rhs.event && code == rhs.code &&
           (substring ? msg.find(rhs.msg) != std::string::npos : msg == rhs.msg);
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

    if (severity != EventSeverity::Debug) {
        messages.emplace_back(severity, event, code, msg);
    }

    return true;
}

bool FixtureLog::Observer::empty() const {
    std::lock_guard<std::mutex> lock(messagesMutex);

    return messages.empty();
}

size_t FixtureLog::Observer::count(const Message& message, bool substring) const {
    std::lock_guard<std::mutex> lock(messagesMutex);

    size_t message_count = 0;
    for (const auto& msg : messages) {
        if (!msg.checked && msg.matches(message, substring)) {
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

size_t FixtureLog::count(const FixtureLog::Message& message, bool substring) const {
    return observer ? observer->count(message, substring) : 0;
}

size_t FixtureLog::uncheckedCount() const {
    return observer ? observer->uncheckedCount() : 0;
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

size_t FixtureLogObserver::uncheckedCount() const {
    std::lock_guard<std::mutex> lock(messagesMutex);

    size_t result = 0;
    for (const auto& msg : messages) {
        if (!msg.checked) {
            result++;
        }
    }
    return result;
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
