#include <mbgl/test/util.hpp>

#include <mbgl/renderer/group_by_layout.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/expression/dsl.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::expression;

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
    using namespace mbgl::style::expression::dsl;
    std::vector<std::unique_ptr<Layer>> layers;
    auto lineLayer = std::make_unique<LineLayer>("a", "source");
    lineLayer->setFilter(Filter(get("property")));
    layers.push_back(std::move(lineLayer));
    layers.push_back(std::make_unique<LineLayer>("b", "source"));
    auto result = groupByLayout(toRenderLayers(layers));
    ASSERT_EQ(2u, result.size());
}

TEST(GroupByLayout, UnrelatedLayout) {
    std::vector<std::unique_ptr<Layer>> layers;
    auto lineLayer = std::make_unique<LineLayer>("a", "source");
    lineLayer->setLineCap(LineCapType::Square);
    layers.push_back(std::move(lineLayer));
    layers.push_back(std::make_unique<LineLayer>("b", "source"));
    auto result = groupByLayout(toRenderLayers(layers));
    ASSERT_EQ(2u, result.size());
}
