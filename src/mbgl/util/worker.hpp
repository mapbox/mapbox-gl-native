#ifndef MBGL_UTIL_WORKER
#define MBGL_UTIL_WORKER

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/thread.hpp>

#include <functional>

namespace mbgl {

class Worker : public mbgl::util::noncopyable {
public:
    using Fn = std::function<void ()>;

    Worker(std::size_t count);
    ~Worker();

    void send(Fn work, Fn after);

private:
    class Impl;
    std::vector<std::unique_ptr<util::Thread<Impl>>> threads;
    std::size_t current = 0;
};

}

#endif
