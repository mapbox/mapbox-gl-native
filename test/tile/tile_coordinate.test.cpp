#include <mbgl/test/util.hpp>

#include <mbgl/map/transform.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/tile_coordinate.hpp>


using namespace mbgl;

TEST(TileCoordinate, FromLatLng) {

    size_t changeCount = 0;
    struct TransformObserver : public mbgl::MapObserver {
        void onCameraWillChange(MapObserver::CameraChangeMode mode) final {
            if (mode == MapObserver::CameraChangeMode::Immediate && cameraWillChangeImmediateCallback) {
                cameraWillChangeImmediateCallback();
            }
        }

        void onCameraDidChange(MapObserver::CameraChangeMode mode) final {
            if (mode == MapObserver::CameraChangeMode::Immediate && cameraDidChangeImmediateCallback) {
                cameraDidChangeImmediateCallback();
            }
        }

        std::function<void()> cameraWillChangeImmediateCallback;
        std::function<void()> cameraDidChangeImmediateCallback;
    };

    TransformObserver observer;
    observer.cameraWillChangeImmediateCallback = [&]() {
        ASSERT_EQ(changeCount, 0u);
        ++changeCount;
    };
    observer.cameraDidChangeImmediateCallback = [&]() {
        ASSERT_EQ(changeCount, 1u);
    };

    Transform transform(observer);

    const double max = util::tileSize;
    transform.resize({ static_cast<uint32_t>(max), static_cast<uint32_t>(max) });

    // Center, top-left, bottom-left, bottom-right, top-right edges.
    std::vector<std::pair<LatLng, ScreenCoordinate>> edges {
        { {}, { max / 2.0, max / 2.0 } },
        { { util::LATITUDE_MAX, -util::LONGITUDE_MAX }, { 0, max } },
        { { -util::LATITUDE_MAX, -util::LONGITUDE_MAX }, { 0, 0 } },
        { { -util::LATITUDE_MAX, util::LONGITUDE_MAX }, { max, 0 } },
        { { util::LATITUDE_MAX, util::LONGITUDE_MAX }, { max, max } },
    };

    for (const auto& pair : edges) {
        const auto& latLng = pair.first;
        const auto& screenCoordinate = pair.second;
        const auto base = TileCoordinate::fromLatLng(0, latLng);

        // 16 is the maximum zoom level where we actually compute placements.
        for (uint8_t integerZoom = 0; integerZoom <= 16; ++integerZoom) {
            const double zoom = integerZoom;
            const double maxTilesPerAxis = std::pow(2.0, zoom);
            const Point<double> tilePoint = {
                latLng.longitude() == 0 ? 0.5 : latLng.longitude() == -util::LONGITUDE_MAX ? 0 : 1.0,
                latLng.latitude()  == 0 ? 0.5 : latLng.latitude()  == -util::LATITUDE_MAX  ? 1.0 : 0,
            };

            const auto fromLatLng = TileCoordinate::fromLatLng(zoom, latLng);
            ASSERT_DOUBLE_EQ(fromLatLng.z, zoom);
            ASSERT_DOUBLE_EQ(fromLatLng.p.x, tilePoint.x * maxTilesPerAxis);
            ASSERT_NEAR(fromLatLng.p.y, tilePoint.y * maxTilesPerAxis, 1.0e-7);

            const auto fromScreenCoordinate = TileCoordinate::fromScreenCoordinate(transform.getState(), zoom, screenCoordinate);
            ASSERT_DOUBLE_EQ(fromScreenCoordinate.z, fromLatLng.z);
            ASSERT_NEAR(fromScreenCoordinate.p.x, fromLatLng.p.x, 0.99);
            ASSERT_NEAR(fromScreenCoordinate.p.y, fromLatLng.p.y, 0.99);

            const auto zoomed = base.zoomTo(zoom);
            ASSERT_DOUBLE_EQ(zoomed.z, zoom);
            ASSERT_DOUBLE_EQ(zoomed.p.x, fromLatLng.p.x);
            ASSERT_DOUBLE_EQ(zoomed.p.y, fromLatLng.p.y);
        }
    }
}

TEST(TileCoordinate, ToGeometryCoordinate) {
    std::vector<Point<double>> edges {
        { 0.5, 0.5 }, { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }
    };

    for (uint8_t zoom = 0; zoom <= 16; ++zoom) {
        uint32_t maxTilesPerAxis = std::pow(2, zoom);
        for (const auto& edge : edges) {
            uint32_t tileX = edge.x == 0 ? 0 : edge.x == 1 ? maxTilesPerAxis - 1 : (maxTilesPerAxis / 2.0) - 1;
            uint32_t tileY = edge.y == 0 ? 0 : edge.y == 1 ? maxTilesPerAxis - 1 : (maxTilesPerAxis / 2.0) - 1;
            UnwrappedTileID unwrapped(0, CanonicalTileID { zoom, tileX, tileY });

            auto tilePointX = ((edge.x * maxTilesPerAxis) - tileX) * util::EXTENT;
            auto tilePointY = ((edge.y * maxTilesPerAxis) - tileY) * util::EXTENT;
            GeometryCoordinate point = TileCoordinate::toGeometryCoordinate(unwrapped, edge);
            ASSERT_DOUBLE_EQ(point.x, tilePointX);
            ASSERT_DOUBLE_EQ(point.y, tilePointY);
        }
    }
}
