#include "shared_thread_pool.hpp"

namespace mbgl {

std::shared_ptr<ThreadPool> sharedThreadPool(std::size_t threadPoolSize) {
    static std::weak_ptr<ThreadPool> weak;
    auto pool = weak.lock();
    if (!pool) {
        weak = pool = std::make_shared<ThreadPool>(threadPoolSize);
    }
    return pool;
}

} // namespace mbgl
