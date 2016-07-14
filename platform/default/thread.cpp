#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/log.hpp>

#include <string>

#include <pthread.h>
#include <sched.h>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    char name[32] = "unknown";
#if defined(__APPLE__)
    pthread_getname_np(pthread_self(), name, sizeof(name));
#elif defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 12)
    pthread_getname_np(pthread_self(), name, sizeof(name));
#endif
#endif
    return name;
}

void setCurrentThreadName(const std::string& name) {
#if defined(__APPLE__)
    pthread_setname_np(name.c_str());
#elif defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 12)
    if (name.size() > 15) { // Linux hard limit (see manpages).
        pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
    } else {
        pthread_setname_np(pthread_self(), name.c_str());
    }
#endif
#endif
    (void)name;
}

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
