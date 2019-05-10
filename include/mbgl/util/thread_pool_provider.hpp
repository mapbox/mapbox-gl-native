#pragma once

#include "mbgl/util/thread_pool.hpp"

namespace mbgl {
    std::shared_ptr<ThreadPool> getThreadPool();
}