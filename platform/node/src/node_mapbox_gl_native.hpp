#pragma once

#include <mbgl/util/run_loop.hpp>

#include <memory>

namespace node_mbgl {

std::shared_ptr<mbgl::util::RunLoop> NodeRunLoop();

}
