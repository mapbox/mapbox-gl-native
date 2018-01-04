#include <mbgl/test/util.hpp>

#include <mbgl/renderer/group_by_layout.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>

using namespace mbgl;
using namespace mbgl::style;

static std::vector<std::unique_ptr<RenderLayer>> toRenderLayers(const std::vector<std::unique_ptr<Layer>>& layers) {
    std::vector<std::unique_ptr<RenderLayer>> result;
    result.reserve(layers.size());
    for (auto& layer : layers) {
        result.push_back(RenderLayer::create(layer->baseImpl));
    }
    return result;
}

TEST(GroupByLayout, Related) {
    std::vector<std::unique_ptr<Layer>> layers;
    layers.push_back(std::make_unique<LineLayer>("a", "source"));
    layers.push_back(std::make_unique<LineLayer>("b", "source"));
    auto result = groupByLayout(toRenderLayers(layers));
    ASSERT_EQ(1u, result.size());
    ASSERT_EQ(2u, result[0].size());
}

TEST(GroupByLayout, UnrelatedType) {
    std::vector<std::unique_ptr<Layer>> layers;
    layers.push_back(std::make_unique<BackgroundLayer>("background"));
    layers.push_back(std::make_unique<CircleLayer>("circle", "source"));
    auto result = groupByLayout(toRenderLayers(layers));
    ASSERT_EQ(2u, result.size());
}

TEST(GroupByLayout, UnrelatedFilter) {
    std::vector<std::unique_ptr<Layer>> layers;
    layers.push_back(std::make_unique<LineLayer>("a", "source"));
    layers.push_back(std::make_unique<LineLayer>("b", "source"));
    layers[0]->as<LineLayer>()->setFilter(EqualsFilter());
    auto result = groupByLayout(toRenderLayers(layers));
    ASSERT_EQ(2u, result.size());
}

TEST(GroupByLayout, UnrelatedLayout) {
    std::vector<std::unique_ptr<Layer>> layers;
    layers.push_back(std::make_unique<LineLayer>("a", "source"));
    layers.push_back(std::make_unique<LineLayer>("b", "source"));
    layers[0]->as<LineLayer>()->setLineCap(LineCapType::Square);
    auto result = groupByLayout(toRenderLayers(layers));
    ASSERT_EQ(2u, result.size());
}
