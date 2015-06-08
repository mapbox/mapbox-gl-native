#include <mbgl/util/worker.hpp>
#include <mbgl/util/work_task.hpp>
#include <mbgl/util/work_request.hpp>
#include <mbgl/platform/platform.hpp>

#include <cassert>
#include <future>

namespace mbgl {

class Worker::Impl {
public:
    Impl(uv_loop_t*, FileSource* fs) {
        // FIXME: Workers should not access the FileSource but it
        // is currently needed because of GlyphsPBF. See #1664.
        util::ThreadContext::setFileSource(fs);
    }

    void doWork(Fn work) {
        work();
    }
};

Worker::Worker(std::size_t count) {
    util::ThreadContext context = {"Worker", util::ThreadType::Worker, util::ThreadPriority::Low};
    for (std::size_t i = 0; i < count; i++) {
        threads.emplace_back(std::make_unique<util::Thread<Impl>>(context, util::ThreadContext::getFileSource()));
    }
}

Worker::~Worker() = default;

std::unique_ptr<WorkRequest> Worker::send(Fn work, Fn after) {
    current = (current + 1) % threads.size();
    return threads[current]->invokeWithResult(&Worker::Impl::doWork, after, work);
}

} // end namespace mbgl
