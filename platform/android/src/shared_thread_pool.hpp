#pragma once

#include <mbgl/util/default_thread_pool.hpp>

namespace mbgl {
namespace android {

// May only be called from the main/UI thread.
inline ThreadPool& sharedThreadPool() {
    static ThreadPool threadPool{ 4, { "Worker" } };
    return threadPool;
}

}
}
