#include <benchmark/benchmark.h>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/map/transform.hpp>

using namespace mbgl;

static const LatLngBounds sanFrancisco =
    LatLngBounds::hull({ 37.6609, -122.5744 }, { 37.8271, -122.3204 });

static void TileCountBounds(benchmark::State& state) {
    std::size_t length = 0;
    while (state.KeepRunning()) {
        auto count = util::tileCount(sanFrancisco, 10);
        length += count;
    }
}

static void TileCoverPitchedViewport(benchmark::State& state) {
    Transform transform;
    transform.resize({ 512, 512 });
    // slightly offset center so that tile order is better defined
    transform.jumpTo(CameraOptions().withCenter(LatLng { 0.1, -0.1 }).withZoom(8.0).withBearing(5.0).withPitch(40.0));

    std::size_t length = 0;
    while (state.KeepRunning()) {
        auto tiles = util::tileCover(transform.getState(), 8);
        length += tiles.size();
    }
}

static void TileCoverBounds(benchmark::State& state) {
    std::size_t length = 0;
    while (state.KeepRunning()) {
        auto tiles = util::tileCover(sanFrancisco, 8);
        length += tiles.size();
    }
}

static const auto geomPolygon = Polygon<double>{
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

static void TileCoverPolygon(benchmark::State& state) {
    std::size_t length = 0;

    while (state.KeepRunning()) {
        auto tiles = util::tileCover(geomPolygon, 8);
        length += tiles.size();
    }
}

static void TileCountPolygon(benchmark::State& state) {
    std::size_t length = 0;

    while (state.KeepRunning()) {
        auto tiles = util::tileCount(geomPolygon, 16);
        length += tiles;
    }
}

BENCHMARK(TileCountBounds);
BENCHMARK(TileCountPolygon);
BENCHMARK(TileCoverPitchedViewport);
BENCHMARK(TileCoverBounds);
BENCHMARK(TileCoverPolygon);

