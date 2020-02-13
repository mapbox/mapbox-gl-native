#import <Foundation/Foundation.h>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/platform/thread.hpp>

#include <pthread.h>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    char name[32] = "unknown";
    pthread_getname_np(pthread_self(), name, sizeof(name));

    return name;
}

void setCurrentThreadName(const std::string& name) {
    std::string qualifiedName = "com.mapbox.mbgl." + name;
    pthread_setname_np(qualifiedName.c_str());
}

void makeThreadLowPriority() {
    [[NSThread currentThread] setThreadPriority:0.0];
}

void setCurrentThreadPriority(double priority) {
    if (priority > 1.0 || priority < 0.0) {
        Log::Warning(Event::General, "Invalid thread priority was provided");
        return;
    }

    [[NSThread currentThread] setThreadPriority:priority];
}

void attachThread() {
}

void detachThread() {
}

}
}
