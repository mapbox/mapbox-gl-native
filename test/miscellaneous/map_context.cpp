#include "../fixtures/util.hpp"

#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>

using namespace mbgl;

TEST(MapContext, DoubleStyleLoad) {
    util::RunLoop runLoop(uv_default_loop());
    std::shared_ptr<HeadlessDisplay> display = std::make_shared<HeadlessDisplay>();
    HeadlessView view(display, 512, 512, 1);
    DefaultFileSource fileSource(nullptr);
    MapData data(view, MapMode::Continuous);
    MapContext context(uv_default_loop(), view, fileSource, data);

    context.setStyleJSON("", "");
    context.setStyleJSON("", "");
}
