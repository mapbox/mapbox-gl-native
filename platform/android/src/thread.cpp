#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>

#include <sys/prctl.h>
#include <sys/resource.h>

// Implementation based on Chromium's platform_thread_android.cc.

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    char name[32] = "unknown";

    if (prctl(PR_GET_NAME, name) == -1) {
        Log::Warning(Event::General, "Couldn't get thread name");
    }

    return name;
}

void setCurrentThreadName(const std::string& name) {
    if (prctl(PR_SET_NAME, name.c_str()) == -1) {
        Log::Warning(Event::General, "Couldn't set thread name");
    }
}

void makeThreadLowPriority() {
    // ANDROID_PRIORITY_LOWEST = 19
    //
    // Supposedly would set the priority for the whole process, but
    // on Linux/Android it only sets for the current thread.
    setpriority(PRIO_PROCESS, 0, 19);
}

} // namespace platform
} // namespace mbgl
