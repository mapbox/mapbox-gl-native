#ifndef MBGL_TEST_FIXTURE_LOG_OBSERVER
#define MBGL_TEST_FIXTURE_LOG_OBSERVER

#include <mbgl/platform/log.hpp>
#include <mbgl/util/optional.hpp>

#include <vector>
#include <cstdarg>
#include <iostream>

namespace mbgl {

class FixtureLogObserver : public Log::Observer {
public:
    struct LogMessage {
        LogMessage(EventSeverity severity_, Event event_, int64_t code_, const std::string &msg_);
        LogMessage();

        bool operator==(const LogMessage &rhs) const;

        const mapbox::util::optional<EventSeverity> severity;
        const mapbox::util::optional<Event> event;
        const mapbox::util::optional<int64_t> code;
        const mapbox::util::optional<std::string> msg;

        mutable bool checked = false;
    };

    ~FixtureLogObserver();

    // Log::Observer implementation
    virtual bool onRecord(EventSeverity severity, Event event, int64_t code, const std::string &msg) override;

    size_t count(const LogMessage &message) const;
    std::vector<LogMessage> unchecked() const;

public:
    std::vector<LogMessage> messages;
};

::std::ostream &operator<<(::std::ostream &os,
                           const std::vector<FixtureLogObserver::LogMessage> &messages);
::std::ostream &operator<<(::std::ostream &os, const FixtureLogObserver::LogMessage &message);

}

#endif
