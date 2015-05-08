#ifndef MBGL_UTIL_WORKER
#define MBGL_UTIL_WORKER

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/thread.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class WorkRequest;

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
    // Its destructor guarantees that the work function has finished executing, and
    // that the after function has either finished executing or will not execute.
    // Together, this means that an object may make a work request with lambdas which
    // bind references to itself, and if and when those lambdas execute, the references
    // will still be valid.
    std::unique_ptr<WorkRequest> send(Fn work, Fn after);

private:
    class Impl;
    std::vector<std::unique_ptr<util::Thread<Impl>>> threads;
    std::size_t current = 0;
};

}

#endif
