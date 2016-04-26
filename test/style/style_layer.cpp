#include <mbgl/test/util.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>

using namespace mbgl;
using namespace mbgl::style;

TEST(Layer, Clone) {
    std::unique_ptr<Layer> layer = std::make_unique<BackgroundLayer>("id");
    std::unique_ptr<Layer> clone = layer->baseImpl->clone();
    EXPECT_NE(layer.get(), clone.get());
    EXPECT_TRUE(reinterpret_cast<BackgroundLayer::Impl*>(clone->baseImpl.get()));
}

TEST(Layer, CloneCopiesBaseProperties) {
    std::unique_ptr<BackgroundLayer> layer = std::make_unique<BackgroundLayer>("id");
    layer->impl->id = "test";
    EXPECT_EQ("test", layer->baseImpl->clone()->getID());
}
