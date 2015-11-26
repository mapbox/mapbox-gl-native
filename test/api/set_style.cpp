#include "../fixtures/util.hpp"
#include "../fixtures/fixture_log_observer.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/storage/default_file_source.hpp>


TEST(API, SetStyle) {
    using namespace mbgl;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    DefaultFileSource fileSource(nullptr);

    Log::setObserver(std::make_unique<FixtureLogObserver>());

    {
        Map map(view, fileSource, MapMode::Still);
        map.setStyleJSON("invalid", "");
    }

    auto observer = Log::removeObserver();
    auto flo = dynamic_cast<FixtureLogObserver*>(observer.get());
    EXPECT_EQ(1ul, flo->count({ EventSeverity::Error, Event::ParseStyle, -1,
        "Error parsing style JSON at 0: Invalid value." }));
    auto unchecked = flo->unchecked();
    EXPECT_TRUE(unchecked.empty()) << unchecked;
}
