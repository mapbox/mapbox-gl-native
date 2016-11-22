#include <mbgl/benchmark/util.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {
namespace benchmark {

void render(Map& map) {
    PremultipliedImage result;
    map.renderStill([&result](std::exception_ptr, PremultipliedImage&& image) {
        result = std::move(image);
    });

    while (!result.size()) {
        util::RunLoop::Get()->runOnce();
    }
}

} // namespace benchmark
} // namespace mbgl
