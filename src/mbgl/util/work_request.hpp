#ifndef MBGL_UTIL_WORK_REQUEST
#define MBGL_UTIL_WORK_REQUEST

#include <mbgl/util/noncopyable.hpp>

#include <memory>

namespace mbgl {

class WorkTask;

class WorkRequest : public util::noncopyable {
public:
    using Task = std::shared_ptr<WorkTask>;
    WorkRequest(Task);
    ~WorkRequest();

private:
    std::shared_ptr<WorkTask> task;
};

}

#endif
