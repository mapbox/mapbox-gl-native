#include <mbgl/test/util.hpp>

#include <mbgl/style/style_layer.hpp>
#include <mbgl/layer/background_layer.hpp>

using namespace mbgl;

TEST(StyleLayer, Create) {
    std::unique_ptr<StyleLayer> layer = std::make_unique<BackgroundLayer>();
    EXPECT_TRUE(reinterpret_cast<BackgroundLayer*>(layer.get()));
}

TEST(StyleLayer, Clone) {
    std::unique_ptr<StyleLayer> layer = std::make_unique<BackgroundLayer>();
    std::unique_ptr<StyleLayer> clone = layer->clone();
    EXPECT_NE(layer.get(), clone.get());
    EXPECT_TRUE(reinterpret_cast<BackgroundLayer*>(layer.get()));
}

TEST(StyleLayer, CloneCopiesBaseProperties) {
    std::unique_ptr<BackgroundLayer> layer = std::make_unique<BackgroundLayer>();
    layer->id = "test";
    EXPECT_EQ("test", layer->clone()->id);
}
