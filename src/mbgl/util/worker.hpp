#ifndef MBGL_UTIL_WORKER
#define MBGL_UTIL_WORKER

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/util/thread.hpp>

#include <functional>

namespace mbgl {

class Worker : public mbgl::util::noncopyable {
public:
    using Fn = std::function<void ()>;

    Worker(std::size_t count);
    ~Worker();

    // Request work be done on a thread pool. The optional after callback is
    // executed on the invoking thread, which must have a run loop, after the
    // work is complete.
    //
    // The return value represents the request to perform the work asynchronously.
    // Its destructor guarantees that the work is either cancelled and will never
    // execute, or has finished executing. In other words, the WorkRequest is
    // guaranteed to outlive any references held by the work function.
    WorkRequest send(Fn work, Fn after);

private:
    class Impl;
    std::vector<std::unique_ptr<util::Thread<Impl>>> threads;
    std::size_t current = 0;
};

}

#endif
