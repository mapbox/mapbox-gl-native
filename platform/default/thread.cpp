#include <mbgl/platform/platform.hpp>

#include <mbgl/platform/log.hpp>

#include <pthread.h>
#ifdef __linux__
#include <linux/sched.h>
#endif
#include <sched.h>

namespace mbgl {
namespace platform {

void makeThreadLowPriority() {
#ifdef SCHED_IDLE
    struct sched_param param;
    param.sched_priority = 0;
    int status = sched_setscheduler(0, SCHED_IDLE, &param);
    if (status != 0) {
        Log::Warning(Event::General, "Couldn't set thread scheduling policy");
    }
#else
#pragma message("Building without thread priority control")
#endif
}

} // namespace platform
} // namespace mbgl
