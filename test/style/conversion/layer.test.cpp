#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/util/rapidjson.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;
using namespace std::literals::chrono_literals;

auto parseLayer(const std::string& src) {
    JSDocument doc;
    doc.Parse<0>(src);
    return convert<std::unique_ptr<Layer>, JSValue>(doc);
}

TEST(StyleConversion, LayerTransition) {
    auto layer = parseLayer(R"JSON({
        "type": "background",
        "id": "background",
        "paint": {
            "background-color-transition": {
                "duration": 400,
                "delay": 500
            }
        },
        "paint.class": {
            "background-color-transition": {
                "duration": 100
            }
        }
    })JSON");

    ASSERT_EQ(400ms, *(*layer)->as<BackgroundLayer>()->impl->paint.cascading
        .get<BackgroundColor>().getTransition({}).duration);
    ASSERT_EQ(500ms, *(*layer)->as<BackgroundLayer>()->impl->paint.cascading
        .get<BackgroundColor>().getTransition({}).delay);

    ASSERT_EQ(100ms, *(*layer)->as<BackgroundLayer>()->impl->paint.cascading
        .get<BackgroundColor>().getTransition({"class"}).duration);
    ASSERT_FALSE(bool((*layer)->as<BackgroundLayer>()->impl->paint.cascading
        .get<BackgroundColor>().getTransition({"class"}).delay));
}
