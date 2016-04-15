#ifndef MBGL_UTIL_WORK_REQUEST
#define MBGL_UTIL_WORK_REQUEST

#include <mbgl/util/async_request.hpp>

#include <memory>

namespace mbgl {

class WorkTask;

class WorkRequest : public AsyncRequest {
public:
    using Task = std::shared_ptr<WorkTask>;
    WorkRequest(Task);
    ~WorkRequest() override;

    bool tryCancel();

private:
    std::shared_ptr<WorkTask> task;
};

} // namespace mbgl

#endif
