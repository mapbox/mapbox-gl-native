#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/mock_file_source.hpp"
#include "../fixtures/util.hpp"

#include <mbgl/annotation/point_annotation.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>

#include <future>
#include <vector>

TEST(API, PointAnnotation) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);

    MockFileSource fileSource(MockFileSource::Success, "");

    Map map(view, fileSource, MapMode::Still);
    map.setStyleURL("test/fixtures/resources/style.json");

    std::vector<PointAnnotation> points;
    points.emplace_back(PointAnnotation({ 50.0, 50.0 }, "default_marker"));

    map.addPointAnnotations(points);

    std::promise<bool> promise;
    map.renderStill([&promise](std::exception_ptr, std::unique_ptr<const StillImage>) {
        promise.set_value(true);
    });

    promise.get_future().get();
}
