#include <mbgl/test/util.hpp>

#include <mbgl/util/image.hpp>
#include <mbgl/geometry/dem_pyramid.hpp>

using namespace mbgl;

auto fakeImage = [](Size s) {
    PremultipliedImage img = PremultipliedImage(s);

    for (size_t i = 0; i < img.bytes(); i ++) {
        img.data[i] = (i+1) % 4 == 0 ? 1 : std::rand() % 255;
    }
    return img;
};

TEST(Level, Constructor) {
    DEMPyramid::Level level(4, 2);
    EXPECT_EQ(level.dim, 4);
    EXPECT_EQ(level.border, 2);
    EXPECT_EQ(level.stride, 8);
    EXPECT_EQ(level.image.bytes(), size_t(8*8*4));
}

TEST(Level, RoundTrip) {
    DEMPyramid::Level level(4, 2);
    level.set(0, 0, 255);
    EXPECT_EQ(level.get(0,0), 255);
}

TEST(DEMPyramid, Constructor) {
    PremultipliedImage image = fakeImage({16, 16});
    DEMPyramid pyramid(image);

    EXPECT_TRUE(pyramid.isLoaded());
    EXPECT_EQ(pyramid.level.dim, 16);
    EXPECT_EQ(pyramid.level.border, 8);
};

TEST(DEMPyramid, InitialBackfill) {

    PremultipliedImage image1 = fakeImage({4, 4});
    DEMPyramid dem1(image1);

    bool nonempty = true;
    // checking that a 1 px border around the fake image has been populated
    // with a non-empty pixel value
    for (int x = -1; x < 5; x++){
        for (int y = -1; y < 5; y ++) {
            if (dem1.level.get(x, y) == -65536) {
                nonempty = false;
                break;
            }
        }
    }
    EXPECT_TRUE(nonempty);

    bool verticalBorderMatch = true;
    int vertx[] = {-1, 4};
    for (int x : vertx) {
        for (int y = 0; y < 4; y++) {
            if (dem1.level.get(x, y) != dem1.level.get(x < 0 ? x + 1 : x - 1, y)) {
                verticalBorderMatch = false;
                break;
            }
        }
    }
    // vertical border of DEM data is initially equal to next column of data
    EXPECT_TRUE(verticalBorderMatch);

    // horizontal borders empty
    bool horizontalBorderMatch = true;
    int horiz[] = {-1, 4};
    for (int y : horiz) {
        for (int x = 0; x < 4; x++) {
            if (dem1.level.get(x, y) != dem1.level.get(x, y < 0 ? y + 1 : y - 1)) {
                horizontalBorderMatch = false;
                break;
            }
        }
    }
    //horizontal border of DEM data is initially equal to next row of data

    EXPECT_TRUE(horizontalBorderMatch);
    // -1, 1 corner initially equal to closest corner data
    EXPECT_TRUE(dem1.level.get(-1, 4) == dem1.level.get(0, 3));
    // 1, 1 corner initially equal to closest corner data
    EXPECT_TRUE(dem1.level.get(4, 4) == dem1.level.get(3, 3));
    // -1, -1 corner initially equal to closest corner data
    EXPECT_TRUE(dem1.level.get(-1, -1) == dem1.level.get(0, 0));
    // -1, 1 corner initially equal to closest corner data
    EXPECT_TRUE(dem1.level.get(4, -1) == dem1.level.get(3, 0));
};

TEST(DEMPyramid, BackfillNeighbor) {
    PremultipliedImage image1 = fakeImage({4, 4});
    DEMPyramid dem0(image1);

    PremultipliedImage image2 = fakeImage({4, 4});
    DEMPyramid dem1(image2);

    dem0.backfillBorder(dem1, -1, 0);
    for (int y = 0; y < 4; y++) {
        // dx = -1, dy = 0, so the left edge of dem1 should equal the right edge of dem0
        // backfills Left neighbor
        EXPECT_TRUE(dem0.level.get(-1, y) == dem1.level.get(3, y));

    }

    dem0.backfillBorder(dem1, 0, -1);
    // backfills TopCenter neighbor
    for (int x = 0; x < 4; x++) {
        EXPECT_TRUE(dem0.level.get(x, -1) == dem1.level.get(x, 3));
    }

    dem0.backfillBorder(dem1, 1, 0);
    // backfills Right neighb// backfulls TopRight neighboror
    for (int y = 0; y < 4; y++) {
        EXPECT_TRUE(dem0.level.get(4, y) == dem1.level.get(0, y));
    }

    dem0.backfillBorder(dem1, 0, 1);
    // backfills BottomCenter neighbor
    for (int x = 0; x < 4; x++) {
        EXPECT_TRUE(dem0.level.get(x, 4) == dem1.level.get(x, 0));
    }

    dem0.backfillBorder(dem1, -1, 1);
    // backfulls TopRight neighbor
    EXPECT_TRUE(dem0.level.get(-1, 4) == dem1.level.get(3, 0));

    dem0.backfillBorder(dem1, 1, 1);
    // backfulls BottomRight neighbor
    EXPECT_TRUE(dem0.level.get(4, 4) == dem1.level.get(0, 0));

    dem0.backfillBorder(dem1, -1, -1);
    // backfulls TopLeft neighbor
    EXPECT_TRUE(dem0.level.get(-1, -1) == dem1.level.get(3, 3));

    dem0.backfillBorder(dem1, 1, -1);
    // backfulls BottomLeft neighbor
    EXPECT_TRUE(dem0.level.get(4, -1) == dem1.level.get(0, 3));
};
