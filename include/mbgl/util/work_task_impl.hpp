#pragma once

#include <mbgl/util/work_task.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mutex>

namespace mbgl {

template <class F, class P>
class WorkTaskImpl : public WorkTask {
public:
    WorkTaskImpl(F f, P p, std::shared_ptr<std::atomic<bool>> canceled_)
      : canceled(std::move(canceled_)),
        func(std::move(f)),
        params(std::move(p)) {
    }

    void operator()() override {
        // Lock the mutex while processing so that cancel() will block.
        std::lock_guard<std::recursive_mutex> lock(mutex);
        if (!*canceled) {
            invoke(std::make_index_sequence<std::tuple_size<P>::value>{});
        }
    }

    // If the task has not yet begun, this will cancel it.
    // If the task is in progress, this will block until it completed. (Currently
    // necessary because of shared state, but should be removed.) It will also
    // cancel the after callback.
    // If the task has completed, but the after callback has not executed, this
    // will cancel the after callback.
    // If the task has completed and the after callback has executed, this will
    // do nothing.
    void cancel() override {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        *canceled = true;
    }

private:
    template <std::size_t... I>
    void invoke(std::index_sequence<I...>) {
        func(std::move(std::get<I>(std::forward<P>(params)))...);
    }

    std::recursive_mutex mutex;
    std::shared_ptr<std::atomic<bool>> canceled;

    F func;
    P params;
};

template <class Fn, class... Args>
std::shared_ptr<WorkTask> WorkTask::make(Fn&& fn, Args&&... args) {
    auto flag = std::make_shared<std::atomic<bool>>();
    *flag = false;

    auto tuple = std::make_tuple(std::forward<Args>(args)...);
    return std::make_shared<WorkTaskImpl<std::decay_t<Fn>, decltype(tuple)>>(
        std::forward<Fn>(fn),
        std::move(tuple),
        flag);
}

} // namespace mbgl
