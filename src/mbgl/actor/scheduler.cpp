#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/thread_local.hpp>
#include <mbgl/util/thread_pool.hpp>

namespace mbgl {

std::function<void()> Scheduler::bindOnce(std::function<void()> fn) {
    assert(fn);
    return [scheduler = makeWeakPtr(), scheduled = std::move(fn)]() mutable {
        if (!scheduled) return; // Repeated call.
        auto schedulerGuard = scheduler.lock();
        if (scheduler) scheduler->schedule(std::move(scheduled));
    };
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
PassRefPtr<Scheduler> Scheduler::GetBackground() {
    static std::weak_ptr<Scheduler> weak;
    static std::mutex mtx;

    std::lock_guard<std::mutex> lock(mtx);
    std::shared_ptr<Scheduler> scheduler = weak.lock();

    if (!scheduler) {
        weak = scheduler = std::make_shared<ThreadPool>();
    }

    return PassRefPtr<Scheduler>(std::move(scheduler));
}

// static
PassRefPtr<Scheduler> Scheduler::GetSequenced() {
    const std::size_t kSchedulersCount = 10;
    static std::vector<std::weak_ptr<Scheduler>> weaks(kSchedulersCount);
    static std::mutex mtx;
    static std::size_t lastUsedIndex = 0u;

    std::lock_guard<std::mutex> lock(mtx);

    if (++lastUsedIndex == kSchedulersCount) lastUsedIndex = 0u;

    std::shared_ptr<Scheduler> result;
    for (std::size_t i = 0; i < kSchedulersCount; ++i) {
        auto& weak = weaks[i];
        if (auto scheduler = weak.lock()) {
            if (lastUsedIndex == i) result = scheduler;
            continue;
        }
        result = std::make_shared<SequencedScheduler>();
        weak = result;
        lastUsedIndex = i;
        break;
    }

    return PassRefPtr<Scheduler>(std::move(result));
}

} //namespace mbgl
