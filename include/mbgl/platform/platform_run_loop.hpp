#pragma once

#include <mbgl/util/run_loop.hpp>

namespace mbgl {

class PlatformRunLoop
public:
    virtual void run() = 0;
    virtual void runOnce() = 0;
    virtual void stop() = 0;

    virtual void addWatch(int fd, Event, std::function<void(int, util::RunLoop::Event)>&& callback) = 0;
    virtual void removeWatch(int fd) = 0;
};

} // namespace mbgl
