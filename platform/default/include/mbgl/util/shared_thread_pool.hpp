#pragma once

#include <mbgl/util/default_thread_pool.hpp>

namespace mbgl {

std::shared_ptr<ThreadPool> sharedThreadPool();

} // namespace mbgl
