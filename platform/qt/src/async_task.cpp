#include "async_task_impl.hpp"

#include <mbgl/util/async_task.hpp>
#include <mbgl/util/run_loop.hpp>

#include <cassert>

namespace mbgl {
namespace util {

AsyncTask::Impl::Impl(std::function<void()>&& fn)
    : runLoop(RunLoop::Get()),
      task(std::move(fn)) {
    connect(this, SIGNAL(send(void)), this, SLOT(runTask(void)), Qt::QueuedConnection);
}

void AsyncTask::Impl::maySend() {
    if (!queued.test_and_set()) {
        emit send();
    }
}

void AsyncTask::Impl::runTask() {
    assert(runLoop == RunLoop::Get());

    queued.clear();
    task();
}

AsyncTask::AsyncTask(std::function<void()>&& fn)
    : impl(std::make_unique<Impl>(std::move(fn))) {
}

AsyncTask::~AsyncTask() {
}

void AsyncTask::send() {
    impl->maySend();
}

}
}
