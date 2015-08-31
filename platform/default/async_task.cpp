#include <mbgl/util/async_task.hpp>

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <atomic>
#include <functional>

namespace mbgl {
namespace util {

class AsyncTask::Impl {
public:
    Impl(std::function<void()>&& fn)
        : async(RunLoop::getLoop(), [this] { runTask(); })
        , task(std::move(fn)) {
    }

    void maySend() {
        async.send();
    }

    void runTask() {
        task();
    }

    void unref() {
        async.unref();
    }

private:
    uv::async async;

    std::function<void()> task;
};

AsyncTask::AsyncTask(std::function<void()>&& fn)
    : impl(std::make_unique<Impl>(std::move(fn))) {
}

AsyncTask::~AsyncTask() {
}

void AsyncTask::send() {
    impl->maySend();
}

void AsyncTask::unref() {
    impl->unref();
}

}
}
