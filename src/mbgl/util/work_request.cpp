#include <mbgl/util/work_request.hpp>
#include <mbgl/util/work_task.hpp>

#include <cassert>
#include <utility>

namespace mbgl {

WorkRequest::WorkRequest(Task task_) : task(std::move(task_)) {
    assert(task);
}

WorkRequest::~WorkRequest() {
    task->cancel();
}

} // namespace mbgl
