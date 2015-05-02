#ifndef MBGL_UTIL_WORK_REQUEST
#define MBGL_UTIL_WORK_REQUEST

#include <mbgl/util/noncopyable.hpp>

#include <future>

namespace mbgl {

class WorkRequest : public util::noncopyable {
public:
    using Future = std::future<void>;

    WorkRequest();
    WorkRequest(Future&&);
    WorkRequest(WorkRequest&&);
    ~WorkRequest();

    WorkRequest& operator=(WorkRequest&&);
    operator bool() const;

    // Wait for the worker task to complete.
    void join();

private:
    Future complete;
};

}

#endif
