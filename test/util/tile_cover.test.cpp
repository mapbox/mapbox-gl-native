#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/map/transform.hpp>

#include <gtest/gtest.h>

using namespace mbgl;

TEST(TileCover, Empty) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}), util::tileCover(LatLngBounds::empty(), 0));
}

TEST(TileCover, Arctic) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::hull({ 86, -180 }, { 90, 180 }), 0));
}

TEST(TileCover, Antarctic) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::hull({ -86, -180 }, { -90, 180 }), 0));
}

TEST(TileCover, WorldZ0) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 0, 0, 0 },
              }),
              util::tileCover(LatLngBounds::world(), 0));
}

TEST(TileCover, Pitch) {
    Transform transform;
    transform.resize({ 512, 512 });
    // slightly offset center so that tile order is better defined
    transform.setLatLng({ 0.1, -0.1 });
    transform.setZoom(2);
    transform.setAngle(5.0);
    transform.setPitch(40.0 * M_PI / 180.0);

    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 2, 1, 2 }, { 2, 1, 1 }, { 2, 2, 2 }, { 2, 2, 1 }, { 2, 3, 2 }
              }),
              util::tileCover(transform.getState(), 2));
}

TEST(TileCover, WorldZ1) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 },
              }),
              util::tileCover(LatLngBounds::world(), 1));
}

TEST(TileCover, SingletonZ0) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::singleton({ 0, 0 }), 0));
}

TEST(TileCover, SingletonZ1) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{}),
              util::tileCover(LatLngBounds::singleton({ 0, 0 }), 1));
}

TEST(TileCoverStream, Arctic) {
    auto bounds = LatLngBounds::hull({ 84, -180 }, { 70, 180 });
    auto zoom = 3;
    util::TileCover tc(bounds, zoom);
    auto results = util::tileCover(bounds, zoom);
    std::vector<UnwrappedTileID> t;
    while(tc.getTiles([&](int32_t x0, int32_t x1, int32_t y) {
        for (auto x = x0; x < x1; x++) {
            t.emplace_back(zoom, x, y);
        }
    })) {};
    EXPECT_EQ(t.size(), results.size());
}

TEST(TileCoverStream, WorldZ1) {
    auto zoom = 1;
    util::TileCover tc(LatLngBounds::world(), zoom);
    std::vector<UnwrappedTileID> t;
    while(tc.getTiles([&](int32_t x0, int32_t x1, int32_t y) {
        for (auto x = x0; x < x1; x++) {
            t.emplace_back(zoom, x, y);
        }
    })){};
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                { 1, 0, 0 }, { 1, 1, 0 }, { 1, 0, 1 }, { 1, 1, 1 },
            }), t);
}

static const LatLngBounds sanFrancisco =
    LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });

TEST(TileCover, SanFranciscoZ0) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 0, 0, 0 },
              }),
              util::tileCover(sanFrancisco, 0));
}

TEST(TileCover, SanFranciscoZ10) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{
                  { 10, 163, 395 }, { 10, 163, 396 }, { 10, 164, 395 }, { 10, 164, 396 },
              }),
              util::tileCover(sanFrancisco, 10));
}

static const LatLngBounds sanFranciscoWrapped =
    LatLngBounds::hull({ 37.6609, 238.5744 }, { 37.8271, 238.3204 });

TEST(TileCover, SanFranciscoZ0Wrapped) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 0, 1, 0 } }),
              util::tileCover(sanFranciscoWrapped, 0));
}

TEST(TileCover, GeomPointZ13) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 13, 2343, 3133 } }),
            util::tileCover(Point<double> {-77.03355114851098,38.89224995264726 }, 13));
}

TEST(TileCover, GeomPointZ10) {
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 10, 292, 391 } }),
            util::tileCover(Point<double> {-77.03355114851098,38.89224995264726 }, 10));
}

TEST(TileCover, GeomLineZ10) {

    auto lineCover = util::tileCover(LineString<double>{
            {-121.49368286132812,38.57903714667459},
            {-122.4422836303711,37.773157169570695}
        }, 10);
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 10, 166, 392}, {10, 165, 393}, {10, 166, 393}, {10, 164, 394}, {10, 165, 394}, {10,163,395}, {10, 164, 395} }),
        lineCover);

}

TEST(TileCover, GeomLineZ13) {
    auto lineCover = util::tileCover(LineString<double>{
            {-77.03342914581299,38.892101707724315},
            {-77.02394485473633,38.89203490311832},
            {-77.02390193939209,38.8824811975508},
            {-77.0119285583496,38.8824811975508},
            {-77.01218605041504,38.887391829071106},
            {-77.01390266418456,38.88735842456116},
            {-77.01622009277342,38.896510672795266},
            {-77.01725006103516,38.914143795902376},
            {-77.01879501342773,38.914143795902376},
            {-77.0196533203125,38.91307524644972}
        }, 13);
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 13, 2343, 3133 }, { 13, 2343, 3134 } }),
        lineCover);
}

TEST(TileCover, WrappedGeomLineZ10) {
    auto lineCover = util::tileCover(LineString<double>{
            {-179.93342914581299,38.892101707724315},
            {-180.02394485473633,38.89203490311832}
        }, 10);
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 10, -1, 391 }, { 10, 0, 391 } }),
        lineCover);

    lineCover = util::tileCover(LineString<double>{
            {179.93342914581299,38.892101707724315},
            {180.02394485473633,38.89203490311832}
        }, 10);
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 10, 1023, 391 }, { 10, 1024, 391 } }),
        lineCover);
}

TEST(TileCover, GeomLineZ15) {
    auto lineCover = util::tileCover(LineString<double>{
            {-77.03342914581299,38.892101707724315},
            {-77.02394485473633,38.89203490311832},
            {-77.02390193939209,38.8824811975508},
            {-77.0119285583496,38.8824811975508},
            {-77.01218605041504,38.887391829071106},
            {-77.01390266418456,38.88735842456116},
            {-77.01622009277342,38.896510672795266},
            {-77.01725006103516,38.914143795902376},
            {-77.01879501342773,38.914143795902376},
            {-77.0196533203125,38.91307524644972}
        }, 15);
    EXPECT_EQ(lineCover, (std::vector<UnwrappedTileID>{
        { 15,9373,12533 },
        { 15,9373,12534 },
        { 15,9372,12535 },
        { 15,9373,12535 },
        { 15,9373,12536 },
        { 15,9374,12536 }
        }));
}

TEST(TileCover, GeomSanFranciscoPoly) {
    auto sanFranciscoGeom = Polygon<double>{
        {
            {-122.5143814086914,37.779127216982424},
            {-122.50811576843262,37.72721239056709},
            {-122.50313758850099,37.70820178063929},
            {-122.3938751220703,37.707454835665274},
            {-122.37567901611328,37.70663997801684},
            {-122.36297607421874,37.71343018466285},
            {-122.354736328125,37.727280276860036},
            {-122.36469268798828,37.73868429065797},
            {-122.38014221191408,37.75442980295571},
            {-122.38391876220702,37.78753873820529},
            {-122.35919952392578,37.8065289741725},
            {-122.35679626464844,37.820632846207864},
            {-122.3712158203125,37.835276322922695},
            {-122.3818588256836,37.82958198283902},
            {-122.37190246582031,37.80788523279169},
            {-122.38735198974608,37.791337175930686},
            {-122.40966796874999,37.812767557570204},
            {-122.46425628662108,37.807071480609274},
            {-122.46803283691405,37.810326435534755},
            {-122.47901916503906,37.81168262440736},
            {-122.48966217041016,37.78916666399649},
            {-122.50579833984375,37.78781006166096},
            {-122.5143814086914,37.779127216982424}
        }
    };

    auto results = util::tileCover(sanFranciscoGeom, 10);
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 10, 163, 395 }, { 10, 163, 396 } }),
        results);

    results = util::tileCover(sanFranciscoGeom, 12);
    EXPECT_EQ((std::vector<UnwrappedTileID>{
            { 12, 654, 1582 }, { 12, 655, 1582 },
            { 12, 654, 1583 },  { 12, 655, 1583 },
            { 12, 654, 1584 }, { 12, 655, 1584 }
        }), results);

}

TEST(TileCover, GeomPoint) {
    auto geom = Point<double>(-122.5744, 37.6609);

    EXPECT_EQ((std::vector<UnwrappedTileID>{ {2 ,0 ,1 } }),
            util::tileCover(geom, 2));
}

TEST(TileCover, GeomMultiPoint) {
    auto geom = MultiPoint<double>{ { -122.5, 37.76 }, { -122.4, 37.76} };

    EXPECT_EQ((std::vector<UnwrappedTileID>{
                {19, 83740, 202675}, {19, 83886, 202675} }),
            util::tileCover(geom, 19));
}

TEST(TileCover, GeomLineString) {
    auto geom = LineString<double>{{ -122.5, 37.76 }, { -122.4, 37.76} };

    EXPECT_EQ((std::vector<UnwrappedTileID>{
                {14, 2616, 6333}, {14, 2617, 6333}, {14, 2618, 6333},
                {14, 2619, 6333}, {14, 2620, 6333}, {14, 2621, 6333} }),
              util::tileCover(geom, 14));
}

TEST(TileCover, GeomMultiLineString) {
    auto geom = MultiLineString<double>{
            { { -122.5, 37.76 }, { -122.4, 37.76} },
            { { -122.5, 37.72 }, { -122.4, 37.72} } };

    EXPECT_EQ((std::vector<UnwrappedTileID>{
                {14, 2616, 6333}, {14, 2617, 6333}, {14, 2618, 6333},
                {14, 2619, 6333}, {14, 2620, 6333}, {14, 2621, 6333},
                {14, 2616, 6335}, {14, 2617, 6335}, {14, 2618, 6335},
                {14, 2619, 6335}, {14, 2620, 6335}, {14, 2621, 6335} }),
            util::tileCover(geom, 14));
}

TEST(TileCount, World) {
    EXPECT_EQ(1u, util::tileCount(LatLngBounds::world(), 0));
    EXPECT_EQ(4u, util::tileCount(LatLngBounds::world(), 1));
}

TEST(TileCount, SanFranciscoZ10) {
    EXPECT_EQ(4u, util::tileCount(sanFrancisco, 10));
}

TEST(TileCount, SanFranciscoWrappedZ10) {
    EXPECT_EQ(4u, util::tileCount(sanFranciscoWrapped, 10));
}

TEST(TileCount, SanFranciscoZ22) {
    EXPECT_EQ(7254450u, util::tileCount(sanFrancisco, 22));
}

TEST(TileCount, BoundsCrossingAntimeridian) {
    auto crossingBounds = LatLngBounds::hull({-20.9615, -214.309}, {19.477, -155.830});

    EXPECT_EQ(1u, util::tileCount(crossingBounds, 0));
    EXPECT_EQ(4u, util::tileCount(crossingBounds, 3));
    EXPECT_EQ(8u, util::tileCount(crossingBounds, 4));
}
