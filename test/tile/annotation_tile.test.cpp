#include <mbgl/test/util.hpp>
#include <mbgl/test/fake_file_source.hpp>

#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/renderer/query.hpp>
#include <mbgl/text/collision_tile.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/text/glyph_manager.hpp>
#include <mbgl/renderer/backend_scope.hpp>
#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/style/style.hpp>

#include <memory>

using namespace mbgl;

class AnnotationTileTest {
public:
    FakeFileSource fileSource;
    TransformState transformState;
    util::RunLoop loop;
    ThreadPool threadPool { 1 };
    style::Style style { loop, fileSource, 1 };
    AnnotationManager annotationManager { style };
    HeadlessBackend backend;
    BackendScope scope { backend };
    ImageManager imageManager;
    GlyphManager glyphManager { fileSource };

    TileParameters tileParameters {
        1.0,
        MapDebugOptions(),
        transformState,
        threadPool,
        fileSource,
        MapMode::Continuous,
        annotationManager,
        imageManager,
        glyphManager,
        0
    };
};

// Don't query stale collision tile
TEST(AnnotationTile, Issue8289) {
    AnnotationTileTest test;
    AnnotationTile tile(OverscaledTileID(0, 0, 0), test.tileParameters);

    auto data = std::make_unique<AnnotationTileData>();
    data->addLayer("test")->addFeature(0, FeatureType::Point, GeometryCollection());

    // Simulate layout and placement of a symbol layer.
    tile.onLayout(GeometryTile::LayoutResult {
        std::unordered_map<std::string, std::shared_ptr<Bucket>>(),
        std::make_unique<FeatureIndex>(),
        std::move(data),
        0
    });

    auto collisionTile = std::make_unique<CollisionTile>(PlacementConfig());

    IndexedSubfeature subfeature { 0, "", "", 0 };
    CollisionFeature feature(GeometryCoordinates(), Anchor(0, 0, 0, 0), -5, 5, -5, 5, 1, 0, style::SymbolPlacementType::Point, subfeature, CollisionFeature::AlignmentType::Curved);
    collisionTile->insertFeature(feature, 0, true);
    collisionTile->placeFeature(feature, false, false);

    tile.onPlacement(GeometryTile::PlacementResult {
        std::unordered_map<std::string, std::shared_ptr<Bucket>>(),
        std::move(collisionTile),
        {},
        {},
        0
    });

    // Simulate a second layout with empty data.
    tile.onLayout(GeometryTile::LayoutResult {
        std::unordered_map<std::string, std::shared_ptr<Bucket>>(),
        std::make_unique<FeatureIndex>(),
        std::make_unique<AnnotationTileData>(),
        0
    });

    std::unordered_map<std::string, std::vector<Feature>> result;
    GeometryCoordinates queryGeometry {{ Point<int16_t>(0, 0) }};
    TransformState transformState;
    RenderedQueryOptions options;

    tile.queryRenderedFeatures(result, queryGeometry, transformState, {}, options);

    EXPECT_TRUE(result.empty());
}

