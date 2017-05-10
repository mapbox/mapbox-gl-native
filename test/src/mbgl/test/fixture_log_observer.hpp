#pragma once

#include <mbgl/util/logging.hpp>

#include <vector>
#include <cstdarg>
#include <mutex>
#include <iostream>

namespace mbgl {

class FixtureLog {
public:
    struct Message {
        Message() = default;
        Message(EventSeverity severity_, Event event_, int64_t code_, std::string msg_);

        bool operator==(const Message& rhs) const;

        const EventSeverity severity {};
        const Event event {};
        const int64_t code {};
        const std::string msg {};

        mutable bool checked = false;
    };

    class Observer : public Log::Observer {
    public:
        using LogMessage = Message;

        Observer(FixtureLog* log = nullptr);
        ~Observer() override;

        // Log::Observer implementation
        bool onRecord(EventSeverity severity,
                              Event event,
                              int64_t code,
                              const std::string& msg) override;

        bool empty() const;
        size_t count(const Message& message) const;
        std::vector<Message> unchecked() const;

    private:
        FixtureLog* log;
        std::vector<Message> messages;
        mutable std::mutex messagesMutex;
    };

    FixtureLog();

    bool empty() const;
    size_t count(const Message& message) const;

    ~FixtureLog();

private:
    Observer* observer;
};

::std::ostream &operator<<(::std::ostream &os,
                           const std::vector<FixtureLog::Observer::LogMessage> &messages);
::std::ostream &operator<<(::std::ostream &os, const FixtureLog::Observer::LogMessage &message);

using FixtureLogObserver = FixtureLog::Observer;

} // namespace mbgl
