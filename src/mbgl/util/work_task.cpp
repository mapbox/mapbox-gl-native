#include <mbgl/util/work_task.hpp>

#include <cassert>

namespace mbgl {

WorkTask::WorkTask(std::function<void()> task_, std::function<void()> after_)
    : task(task_), after(after_) {
    assert(after);
}

void WorkTask::runTask() {
    // We are only running the task when there's an after callback to call. This means that an
    // empty after callback will be treated as a cancelled request. The mutex will be locked while
    // processing so that the cancel() callback will block.
    std::lock_guard<std::mutex> lock(mutex);
    if (after) {
        task();
    }
}

void WorkTask::runAfter() {
    if (after) {
        after();
    }
}

void WorkTask::cancel() {
    // Remove the after callback to indicate that this callback has been canceled. The mutex will
    // block when the task is currently in progres. When the task has not begun yet, the runTask()
    // method will not do anything. When the task has been completed already, and the after callback
    // was run as well, this will also do nothing.
    std::lock_guard<std::mutex> lock(mutex);
    after = {};
}

} // end namespace mbgl
