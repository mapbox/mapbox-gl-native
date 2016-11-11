#include <mbgl/benchmark/util.hpp>
#include <mbgl/platform/default/offscreen_view.hpp>

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {
namespace benchmark {

void render(Map& map, OffscreenView& view) {
    PremultipliedImage result;
    map.renderStill(view, [&](std::exception_ptr) {
        result = view.readStillImage();
    });

    while (!result.size()) {
        util::RunLoop::Get()->runOnce();
    }
}

} // namespace benchmark
} // namespace mbgl
