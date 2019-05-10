#include <mbgl/util/thread_pool_provider.hpp>

namespace mbgl {
    std::shared_ptr<ThreadPool> getThreadPool() {
        return std::make_shared<ThreadPool>(4);
    }
} // namespace mbgl