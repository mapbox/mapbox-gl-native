#ifndef MBGL_UTIL_WORK_TASK
#define MBGL_UTIL_WORK_TASK

#include <mbgl/util/noncopyable.hpp>

#include <functional>
#include <mutex>

namespace mbgl {

class WorkTask : private util::noncopyable {
public:
    WorkTask(std::function<void()> task, std::function<void()> after);

    void runTask();
    void runAfter();
    void cancel();

private:
    const std::function<void()> task;
    std::function<void()> after;
    std::mutex mutex;
};

} // end namespace mbgl

#endif
