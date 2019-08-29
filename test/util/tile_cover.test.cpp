#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/map/transform.hpp>

#include <algorithm>
#include <cstdlib>     /* srand, rand */
#include <ctime>       /* time */
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

    transform.jumpTo(CameraOptions().withCenter(LatLng { 0.1, -0.1, }).withZoom(2.0).withBearing(5.0).withPitch(40.0));

    EXPECT_EQ((std::vector<UnwrappedTileID>{
        { 2, 1, 1 }, { 2, 2, 1 }, { 2, 1, 2 }, { 2, 2, 2 }
    }),
              util::tileCover(transform.getState(), 2));
}

TEST(TileCover, PitchIssue15442) {
    Transform transform;
    transform.resize({ 412, 691 });

    transform.jumpTo(CameraOptions().withCenter(LatLng { 59.116898740996106, 91.565660781803615, })
        .withZoom(2.0551126748417214).withBearing(0.74963938256567264 * util::RAD2DEG)
        .withPitch(1.0471975511965976 * util::RAD2DEG));

    EXPECT_EQ((std::vector<UnwrappedTileID>{
        { 2, 3, 1 }, { 2, 2, 1 }, { 2, 3, 0 }, { 2, 2, 0 }, { 1, { 2, 0, 0 } }, { 1, { 2, 1, 0 } }
    }),
              util::tileCover(transform.getState(), 2));
}

TEST(TileCover, PitchOverAllowedByContentInsets) {
    Transform transform;
    transform.resize({ 512, 512 });

    transform.jumpTo(CameraOptions().withCenter(LatLng { 0.1, -0.1 }).withPadding(EdgeInsets { 376, 0, 0, 0 })
                                    .withZoom(8.0).withBearing(45.0).withPitch(60.0));
    // Top padding of 376 leads to capped pitch. See Transform::getMaxPitchForEdgeInsets.
    EXPECT_LE(transform.getPitch() + 0.001, util::DEG2RAD * 60);

    EXPECT_EQ((std::vector<UnwrappedTileID>{
        { 3, 4, 3 }, { 3, 3, 3 }, { 3, 4, 4 }, { 3, 3, 4 }, { 3, 4, 2 }, { 3, 5, 3 }, { 3, 5, 2 }
    }),
              util::tileCover(transform.getState(), 3));
}

TEST(TileCover, PitchWithLargerResultSet) {
    Transform transform;
    transform.resize({ 1024, 768 });

    // The values used here triggered the regression with left and right edge
    // selection in tile_cover.cpp scanSpans.
    transform.jumpTo(CameraOptions().withCenter(LatLng { 0.1, -0.1 }).withPadding(EdgeInsets { 400, 0, 0, 0 })
                                    .withZoom(5).withBearing(-142.2630000003529176).withPitch(60.0));

    auto cover = util::tileCover(transform.getState(), 5);
    // Returned vector has above 100 elements, we check first 16 as there is a
    // plan to return lower LOD for distant tiles.
    EXPECT_EQ((std::vector<UnwrappedTileID> {
        { 5, 15, 16 }, { 5, 15, 17 }, { 5, 14, 16 }, { 5, 14, 17 },
        { 5, 16, 16 }, { 5, 16, 17 }, { 5, 15, 15 }, { 5, 14, 15 },
        { 5, 15, 18 }, { 5, 14, 18 }, { 5, 16, 15 }, { 5, 13, 16 },
        { 5, 13, 17 }, { 5, 16, 18 }, { 5, 13, 18 }, { 5, 15, 19 }
    }), (std::vector<UnwrappedTileID> { cover.begin(), cover.begin() + 16}) );
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
    while(tc.hasNext()) {
            t.push_back(*tc.next());
    };
    EXPECT_EQ(t.size(), results.size());
}

TEST(TileCoverStream, WorldZ1) {
    auto zoom = 1;
    util::TileCover tc(LatLngBounds::world(), zoom);
    std::vector<UnwrappedTileID> t;
    while(tc.hasNext()) {
        t.push_back(*tc.next());
    };
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

TEST(TileCover, GeomPoint) {
    auto point = Point<double>{ -122.5744, 37.6609 };

    EXPECT_EQ((std::vector<UnwrappedTileID>{ {2 ,0 ,1 } }),
              util::tileCover(point, 2));
}

TEST(TileCover, GeomMultiPoint) {
    auto points = MultiPoint<double>{ { -122.5, 37.76 }, { -122.4, 37.76} };

    EXPECT_EQ((std::vector<UnwrappedTileID>{
        {20, 167480, 405351}, {20, 167772, 405351} }),
              util::tileCover(points, 20));
}

TEST(TileCover, GeomLineZ10) {
    auto lineCover = util::tileCover(LineString<double>{
        {-121.49368286132812,38.57903714667459},
        {-122.4422836303711,37.773157169570695}
    }, 10);
    EXPECT_EQ((std::vector<UnwrappedTileID>{
        { 10, 166, 392}, {10, 165, 393}, {10, 166, 393},
        {10, 164, 394}, {10, 165, 394}, {10,163,395}, {10, 164, 395}
    }),lineCover);
    
}

TEST(TileCover, GeomLineRegression11870) {
    auto lineCover = util::tileCover(LineString<double>{
        {-121.5063900000001,40.470099999999945},
        {-121.5065300000001,40.470369999999946},
        {-121.5065900000001,40.470519999999944},
    }, 14);
    EXPECT_EQ((std::vector<UnwrappedTileID>{ { 14, 2662, 6174 } }),
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

TEST(TileCover, GeomMultiLineString) {
    auto geom = MultiLineString<double>{
        { { -122.5, 37.76 }, { -122.4, 37.76} },
        { { -122.5, 37.72 }, { -122.4, 37.72} } };

    EXPECT_EQ((std::vector<UnwrappedTileID>{
        {14, 2616, 6333}, {14, 2617, 6333}, {14, 2618, 6333},
        {14, 2619, 6333}, {14, 2620, 6333}, {14, 2621, 6333},
        {14, 2616, 6335}, {14, 2617, 6335}, {14, 2618, 6335},
        {14, 2619, 6335}, {14, 2620, 6335}, {14, 2621, 6335}}),
              util::tileCover(geom, 14));
}

TEST(TileCover, GeomPolygon) {
    auto polygon = Polygon<double>{
        {
            {5.09765625,53.067626642387374},
            {2.373046875,43.389081939117496},
            {-4.74609375,48.45835188280866},
            {-1.494140625,37.09023980307208},
            {22.587890625,36.24427318493909},
            {31.640625,46.13417004624326},
            {17.841796875,54.7246201949245},
            {5.09765625,53.067626642387374},
        },{
            {19.6875,49.66762782262194},
            {22.8515625,43.51668853502906},
            {13.623046875,45.089035564831036},
            {16.34765625,39.095962936305476},
            {5.185546875,41.244772343082076},
            {8.701171874999998,50.233151832472245},
            {19.6875,49.66762782262194}
        }
    };

    auto results = util::tileCover(polygon, 8);

    EXPECT_NE(std::find(results.begin(), results.end(), UnwrappedTileID{8, 134, 87}), results.end());
    EXPECT_NE(std::find(results.begin(), results.end(), UnwrappedTileID{8, 139, 87}), results.end());
    // Should have a hole
    EXPECT_EQ(std::find(results.begin(), results.end(), UnwrappedTileID{8, 136, 87}), results.end());
}

TEST(TileCover, GeomMultiPolygon) {
    auto multiPolygon = MultiPolygon<double>{
        {{
            {5.09765625,53.067626642387374},
            {2.373046875,43.389081939117496},
            {-4.74609375,48.45835188280866},
            {-1.494140625,37.09023980307208},
            {22.587890625,36.24427318493909},
            {31.640625,46.13417004624326},
            {17.841796875,54.7246201949245},
            {5.09765625,53.067626642387374},
        }},{{
            {59.150390625,45.460130637921004},
            {65.126953125,41.11246878918088},
            {69.169921875,47.45780853075031},
            {63.896484375,50.064191736659104},
            {59.150390625,45.460130637921004}
        }}
    };
    auto results = util::tileCover(multiPolygon, 8);

    EXPECT_EQ(424u, results.size());
    EXPECT_NE(std::find(results.begin(), results.end(), UnwrappedTileID{8, 139, 87}), results.end());
    EXPECT_NE(std::find(results.begin(), results.end(), UnwrappedTileID{8, 136, 87}), results.end());
    EXPECT_NE(std::find(results.begin(), results.end(), UnwrappedTileID{8, 174, 94}), results.end());
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

    EXPECT_EQ((std::vector<UnwrappedTileID>{
        { 12, 654, 1582 }, { 12, 655, 1582 },
        { 12, 654, 1583 },  { 12, 655, 1583 },
        { 12, 654, 1584 }, { 12, 655, 1584 }
    }), util::tileCover(sanFranciscoGeom, 12));
}

TEST(TileCover, GeomInvalid) {
    auto point = Point<double>{ -122.5744, 97.6609 };
    EXPECT_THROW(util::tileCover(point, 2), std::domain_error);

    auto badLine = LineString<double>{ {1.0,  35.0} };
    EXPECT_EQ((std::vector<UnwrappedTileID>{ }), util::tileCover(badLine, 16));

    auto badPoly = Polygon<double> { { {1.0,  35.0} } };
    EXPECT_EQ((std::vector<UnwrappedTileID>{ }), util::tileCover(badPoly, 16));

    //Should handle open polygons.
    badPoly = Polygon<double> { { {1.0,  34.2}, {1.0, 34.4}, {0.5, 34.3} } };
    EXPECT_EQ((std::vector<UnwrappedTileID>{
        { 10, 513, 407 }, { 10, 514, 407},
        { 10, 513, 408 }, { 10, 514, 408}
    }), util::tileCover(badPoly, 10));
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

TEST(TileCover, DISABLED_FuzzPoly) {
    while(true)
    {
        std::srand (time(nullptr));
        std::size_t len = std::rand() % 10000 + 3;
        Polygon<double> polygon;

        std::size_t num_rings = 1;
        num_rings += std::rand() % 5;
        while (num_rings > 0) {
            LinearRing<double> ring;
            for (std::size_t i = 0; i < len; ++i) {
                double x = std::rand() % 180;
                double y = std::rand() % 90;

                ring.push_back({x,y});
            }
            polygon.emplace_back(ring);
            --num_rings;
        }
        
        std::clog << ".";
        util::TileCover tc(polygon, 5);
        while(tc.next()) {
        };
    }
}

TEST(TileCover, DISABLED_FuzzLine) {
    while(true)
    {
        std::srand (time(nullptr));
        std::size_t len = std::rand() % 10000 + 3;
        MultiLineString<double> mls;

        std::size_t num_lines = 1;
        num_lines += std::rand() % 5;
        while (num_lines > 0) {
            LineString<double> line;
            for (std::size_t i = 0; i < len; ++i) {
                double x = std::rand() % 180;
                double y = std::rand() % 90;

                line.push_back({x,y});
            }
            mls.emplace_back(line);
            --num_lines;
        }
        
        std::clog << ".";
        util::TileCover tc(mls, 5);
        while(tc.next()) {
        };
    }
}
