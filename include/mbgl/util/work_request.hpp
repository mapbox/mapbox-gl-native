#pragma once

#include <mbgl/util/async_request.hpp>

#include <memory>

namespace mbgl {

class WorkTask;

class WorkRequest : public AsyncRequest {
public:
    using Task = std::shared_ptr<WorkTask>;
    WorkRequest(Task);
    ~WorkRequest() override;

private:
    std::shared_ptr<WorkTask> task;
};

} // namespace mbgl
