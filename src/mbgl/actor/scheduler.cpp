#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/thread_local.hpp>
#include <mbgl/util/thread_pool.hpp>

namespace mbgl {

namespace {

class BindContext {
public:
    BindContext(std::function<void()> scheduled_, mapbox::base::WeakPtr<Scheduler> scheduler_)
        : scheduled(std::move(scheduled_)), scheduler(std::move(scheduler_)) {}

    std::function<void()> scheduled;
    mapbox::base::WeakPtr<Scheduler> scheduler;

    void scheduleOnce() {
        if (!scheduled) return; // Repeated call.
        auto schedulerGuard = scheduler.lock();
        if (scheduler) scheduler->schedule(std::move(scheduled));
        assert(!scheduled);
    }
};

} // namespace

std::function<void()> Scheduler::bindOnce(std::function<void()> fn) {
    assert(fn);
    auto ctx = new BindContext(std::move(fn), makeWeakPtr());
    return [ctx]() { ctx->scheduleOnce(); };
}

static auto& current() {
    static util::ThreadLocal<Scheduler> scheduler;
    return scheduler;
};

void Scheduler::SetCurrent(Scheduler* scheduler) {
    current().set(scheduler);
}

Scheduler* Scheduler::GetCurrent() {
    return current().get();
}

// static
std::shared_ptr<Scheduler> Scheduler::GetBackground() {
    static std::weak_ptr<Scheduler> weak;
    static std::mutex mtx;

    std::lock_guard<std::mutex> lock(mtx);
    std::shared_ptr<Scheduler> scheduler = weak.lock();

    if (!scheduler) {
        weak = scheduler = std::make_shared<ThreadPool>();
    }

    return scheduler;
}

} //namespace mbgl
