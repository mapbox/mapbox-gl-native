#include <mbgl/actor/scheduler.hpp>
#include <mbgl/util/thread_local.hpp>

namespace mbgl {
    
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

} //namespace mbgl
