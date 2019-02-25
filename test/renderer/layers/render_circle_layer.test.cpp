#include <mbgl/test/util.hpp>

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/tile.hpp>

using namespace mbgl;
using namespace mbgl::style;

class MockBucket final : public Bucket {
public:
    using Bucket::Bucket;
    void upload(gl::Context&) {}
    bool hasData() const { return false; }
    bool supportsLayer(const style::Layer::Impl&) const { return true; }
};

class MockTile final : public Tile {
public:
    MockTile(OverscaledTileID id) : Tile(Tile::Kind::Geometry, id) {
        renderable = true;
        bucket = std::make_shared<MockBucket>();
    }
    void upload(gl::Context&){}
    Bucket* getBucket(const style::Layer::Impl&) const {return bucket.get();}

    std::shared_ptr<MockBucket> bucket;
};

struct MockRenderTileData {
    std::vector<std::reference_wrapper<RenderTile>> getRenderTiles() {
        return {renderTiles.begin(), renderTiles.end()};
    }
    std::list<RenderTile> renderTiles;
    std::vector<std::unique_ptr<MockTile>> tiles;
};

template<typename ...Ts>
MockRenderTileData generateRenderTileData(Ts&&... tileIds) {
    MockRenderTileData data;
    for (auto&& tileId : { tileIds... }) {
        auto tile = std::make_unique<MockTile>(tileId.overscaleTo(tileId.canonical.z));
        data.renderTiles.push_back(RenderTile(tileId, *tile));
        data.tiles.emplace_back(std::move(tile));
    }
    return data;
}

TEST(RenderCircleLayer, FilterRenderTiles) {
    auto layer = std::make_unique<CircleLayer>("circle", "source");
    auto renderLayer = std::make_unique<RenderCircleLayer>(layer->mutableImpl());

    // One parent overlaps 4 children.
    auto one = generateRenderTileData(UnwrappedTileID{0, 0, 0},
                                      UnwrappedTileID{1, 0, 0},
                                      UnwrappedTileID{1, 0, 1},
                                      UnwrappedTileID{1, 1, 0},
                                      UnwrappedTileID{1, 1, 1});

    ASSERT_EQ(renderLayer->filterRenderTiles(one.getRenderTiles()).size(), 1);

    // Two parent tiles overlap 5 child tiles.
    auto two = generateRenderTileData(UnwrappedTileID{0, 0, 0},
                                      UnwrappedTileID{1, 0, 0},
                                      UnwrappedTileID{1, 0, 1},
                                      UnwrappedTileID{1, 1, 0},
                                      UnwrappedTileID{1, 1, 1},
                                      UnwrappedTileID{0, 0, 0},
                                      UnwrappedTileID{1, 1, 1});

    ASSERT_EQ(renderLayer->filterRenderTiles(two.getRenderTiles()).size(), 2);

    // Three parent tiles overlapping 3 child tiles.
    auto three = generateRenderTileData(UnwrappedTileID{13,4662,2369},
                                        UnwrappedTileID{13,4662,2370},
                                        UnwrappedTileID{13,4662,2371},
                                        UnwrappedTileID{13,4663,2371},
                                        UnwrappedTileID{14,9325,4740},
                                        UnwrappedTileID{14,9326,4739},
                                        UnwrappedTileID{14,9326,4740},
                                        UnwrappedTileID{14,9326,4741},
                                        UnwrappedTileID{14,9326,4742},
                                        UnwrappedTileID{14,9327,4739},
                                        UnwrappedTileID{14,9327,4740},
                                        UnwrappedTileID{14,9327,4741},
                                        UnwrappedTileID{14,9327,4742},
                                        UnwrappedTileID{14,9328,4739},
                                        UnwrappedTileID{14,9328,4740});

    ASSERT_EQ(renderLayer->filterRenderTiles(three.getRenderTiles()).size(), 12);

    // One parent that overlaps 4 children tiles.
    auto four = generateRenderTileData(UnwrappedTileID{10,583,296},
                                       UnwrappedTileID{14,9324,4739},
                                       UnwrappedTileID{14,9324,4740},
                                       UnwrappedTileID{14,9325,4739},
                                       UnwrappedTileID{14,9325,4740},
                                       UnwrappedTileID{14,9325,4741},
                                       UnwrappedTileID{14,9325,4742},
                                       UnwrappedTileID{14,9325,4743},
                                       UnwrappedTileID{14,9326,4739},
                                       UnwrappedTileID{14,9326,4740},
                                       UnwrappedTileID{14,9326,4741},
                                       UnwrappedTileID{14,9326,4742},
                                       UnwrappedTileID{14,9326,4743},
                                       UnwrappedTileID{14,9327,4739},
                                       UnwrappedTileID{14,9327,4740},
                                       UnwrappedTileID{14,9327,4741},
                                       UnwrappedTileID{14,9327,4742},
                                       UnwrappedTileID{14,9327,4743},
                                       UnwrappedTileID{14,9328,4739},
                                       UnwrappedTileID{14,9328,4740},
                                       UnwrappedTileID{14,9328,4741},
                                       UnwrappedTileID{14,9328,4742});

    ASSERT_EQ(renderLayer->filterRenderTiles(four.getRenderTiles()).size(), 18);
}
