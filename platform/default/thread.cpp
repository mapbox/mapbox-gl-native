#include <mbgl/util/platform.hpp>
#include <mbgl/util/logging.hpp>

#include <string>

#include <pthread.h>
#include <sched.h>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    char name[32] = "unknown";
    pthread_getname_np(pthread_self(), name, sizeof(name));

    return name;
}

void setCurrentThreadName(const std::string& name) {
    if (name.size() > 15) { // Linux hard limit (see manpages).
        pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
    } else {
        pthread_setname_np(pthread_self(), name.c_str());
    }
}

void makeThreadLowPriority() {
    struct sched_param param;
    param.sched_priority = 0;

    if (sched_setscheduler(0, SCHED_IDLE, &param) != 0) {
        Log::Warning(Event::General, "Couldn't set thread scheduling policy");
    }
}

} // namespace platform
} // namespace mbgl
