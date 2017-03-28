#pragma once

#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

std::shared_ptr<ThreadPool> sharedThreadPool(
    std::size_t threadPoolSize = mbgl::util::DEFAULT_SHARED_THREAD_POOL_SIZE);

} // namespace mbgl
