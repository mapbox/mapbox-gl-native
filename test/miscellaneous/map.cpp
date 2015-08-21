#include "../fixtures/util.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>

using namespace mbgl;

TEST(Map, PauseResume) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Continuous);

    map.pause();
    map.resume();
}

TEST(Map, DoublePause) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Continuous);

    map.pause();
    map.pause();
    map.resume();
}

TEST(Map, ResumeWithoutPause) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Continuous);

    map.resume();
}

TEST(Map, DestroyPaused) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Map map(view, fileSource, MapMode::Continuous);

    map.pause();
}
