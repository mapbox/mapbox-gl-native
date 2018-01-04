#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;
using namespace std::literals::chrono_literals;

std::unique_ptr<Layer> parseLayer(const std::string& src) {
    Error error;
    return std::move(*convertJSON<std::unique_ptr<Layer>>(src, error));
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
        }
    })JSON");

    ASSERT_EQ(400ms, *layer->as<BackgroundLayer>()->impl().paint
        .get<BackgroundColor>().options.duration);
    ASSERT_EQ(500ms, *layer->as<BackgroundLayer>()->impl().paint
        .get<BackgroundColor>().options.delay);
}
