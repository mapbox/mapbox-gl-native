#include <mbgl/util/platform.hpp>
#include <mbgl/platform/thread.hpp>
#include <mbgl/util/logging.hpp>

#include <string>

#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    char name[32] = "unknown";
#ifdef __linux__
    pthread_getname_np(pthread_self(), name, sizeof(name));
#endif

    return name;
}

void setCurrentThreadName(const std::string& name) {
#ifdef __linux__
    if (name.size() > 15) { // Linux hard limit (see manpages).
        pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
    } else {
        pthread_setname_np(pthread_self(), name.c_str());
    }
#endif
}

void makeThreadLowPriority() {
#ifdef SCHED_IDLE
    struct sched_param param;
    param.sched_priority = 0;

    if (sched_setscheduler(0, SCHED_IDLE, &param) != 0) {
        Log::Warning(Event::General, "Couldn't set thread scheduling policy");
    }
#endif
}

void setCurrentThreadPriority(double priority) {
    if (setpriority(PRIO_PROCESS, 0, int(priority)) < 0) {
        Log::Warning(Event::General, "Couldn't set thread priority");
    }

#ifdef SCHED_OTHER
    struct sched_param param;
    param.sched_priority = 0;
    if (sched_setscheduler(0, SCHED_OTHER, &param) != 0) {
        Log::Warning(Event::General, "Couldn't set thread scheduling policy");
    }
#endif
}

void attachThread() {}

void detachThread() {}

} // namespace platform
} // namespace mbgl
