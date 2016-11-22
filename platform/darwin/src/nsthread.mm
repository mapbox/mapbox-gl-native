#import <Foundation/Foundation.h>

#include <mbgl/util/platform.hpp>

#include <pthread.h>

namespace mbgl {
namespace platform {

std::string getCurrentThreadName() {
    char name[32] = "unknown";
    pthread_getname_np(pthread_self(), name, sizeof(name));

    return name;
}

void setCurrentThreadName(const std::string& name) {
    pthread_setname_np(name.c_str());
}

void makeThreadLowPriority() {
    [[NSThread currentThread] setThreadPriority:0.0];
}

}
}
