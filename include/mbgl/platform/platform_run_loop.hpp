#pragma once

#include <functional>

namespace mbgl {

class PlatformRunLoop {
public:
    virtual ~PlatformRunLoop() = default;

    virtual void *handle() = 0;

    virtual void run() = 0;
    virtual void runOnce() = 0;
    virtual void stop() = 0;

    enum class Event : uint8_t {
        None      = 0,
        Read      = 1,
        Write     = 2,
        ReadWrite = Read | Write,
    };

    virtual void addWatch(int fd, Event, std::function<void(int, Event)>&& callback) = 0;
    virtual void removeWatch(int fd) = 0;
};

} // namespace mbgl
