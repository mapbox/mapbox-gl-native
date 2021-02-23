#pragma once

#include <functional>

namespace mbgl {

int runRenderTests(int argc, char* argv[], std::function<void()>);

} // namespace mbgl
