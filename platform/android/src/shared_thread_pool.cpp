#include <mbgl/util/shared_thread_pool.hpp>
#include "android_thread_pool.hpp"

namespace mbgl {

std::shared_ptr<ThreadPool> sharedThreadPool() {
    static std::weak_ptr<ThreadPool> weak;
    auto pool = weak.lock();
    if (!pool) {
        weak = pool = std::make_shared<android::AndroidThreadPool>(4);
    }
    return pool;
}

} // namespace mbgl
