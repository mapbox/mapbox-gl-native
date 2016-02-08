#include "../fixtures/util.hpp"

#include <mbgl/map/map_data.hpp>
#include <mbgl/map/map_context.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/util/thread.hpp>

using namespace mbgl;

TEST(MapContext, DoubleStyleLoad) {
    std::shared_ptr<HeadlessDisplay> display = std::make_shared<HeadlessDisplay>();
    HeadlessView view(display, 1, 512, 512);
    OnlineFileSource fileSource(nullptr);

    util::Thread<MapContext> context({"Map", util::ThreadType::Map, util::ThreadPriority::Regular},
        view, fileSource, MapMode::Continuous, GLContextMode::Unique, view.getPixelRatio());

    context.invokeSync(&MapContext::setStyleJSON, "", "");
    context.invokeSync(&MapContext::setStyleJSON, "", "");
    context.invokeSync(&MapContext::cleanup);
}
