#include <mbgl/util/work_request.hpp>
#include <mbgl/util/work_task.hpp>

#include <cassert>

namespace mbgl {

WorkRequest::WorkRequest(Task task_) : task(task_) {
    assert(task);
}

WorkRequest::~WorkRequest() {
    task->cancel();
}

}
