#include <mbgl/benchmark/util.hpp>
#include <mbgl/gl/offscreen_view.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {
namespace benchmark {

void render(Map& map, OffscreenView& view) {
    PremultipliedImage result;
    map.renderStill([&](std::exception_ptr) {
        result = view.readStillImage();
    });

    while (!result.valid()) {
        util::RunLoop::Get()->runOnce();
    }
}

} // namespace benchmark
} // namespace mbgl
