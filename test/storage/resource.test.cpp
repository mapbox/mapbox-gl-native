#include <mbgl/storage/resource.hpp>

#include <gtest/gtest.h>

TEST(Resource, Style) {
    using namespace mbgl;
    Resource resource = Resource::style("http://example.com");
    EXPECT_EQ(Resource::Kind::Style, resource.kind);
    EXPECT_EQ("http://example.com", resource.url);
}

TEST(Resource, Source) {
    using namespace mbgl;
    Resource resource = Resource::source("http://example.com");
    EXPECT_EQ(Resource::Kind::Source, resource.kind);
    EXPECT_EQ("http://example.com", resource.url);
}

TEST(Resource, Tile) {
    using namespace mbgl;

    Resource rasterTile = Resource::tile("http://example.com/{z}/{x}/{y}{ratio}.png", 2.0, 1, 2, 3, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, rasterTile.kind);
    EXPECT_EQ("http://example.com/3/1/2@2x.png", rasterTile.url);
    EXPECT_EQ("http://example.com/{z}/{x}/{y}{ratio}.png", rasterTile.tileData->urlTemplate);
    EXPECT_EQ(2, rasterTile.tileData->pixelRatio);
    EXPECT_EQ(1, rasterTile.tileData->x);
    EXPECT_EQ(2, rasterTile.tileData->y);
    EXPECT_EQ(3, rasterTile.tileData->z);

    Resource vectorTile = Resource::tile("http://example.com/{z}/{x}/{y}.mvt", 2.0, 1, 2, 3, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, vectorTile.kind);
    EXPECT_EQ("http://example.com/3/1/2.mvt", vectorTile.url);
    EXPECT_EQ("http://example.com/{z}/{x}/{y}.mvt", vectorTile.tileData->urlTemplate);
    EXPECT_EQ(1, vectorTile.tileData->pixelRatio);
    EXPECT_EQ(1, vectorTile.tileData->x);
    EXPECT_EQ(2, vectorTile.tileData->y);
    EXPECT_EQ(3, vectorTile.tileData->z);

    Resource quadTile = Resource::tile("http://example.com/{quadkey}.png", 2.0, 0, 0, 1, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, quadTile.kind);
    EXPECT_EQ("http://example.com/0.png", quadTile.url);
    EXPECT_EQ("http://example.com/{quadkey}.png", quadTile.tileData->urlTemplate);
    EXPECT_EQ(1, quadTile.tileData->pixelRatio);
    EXPECT_EQ(0, quadTile.tileData->x);
    EXPECT_EQ(0, quadTile.tileData->y);
    EXPECT_EQ(1, quadTile.tileData->z);

    quadTile = Resource::tile("http://example.com/{quadkey}.png", 2.0, 0, 0, 2, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, quadTile.kind);
    EXPECT_EQ("http://example.com/00.png", quadTile.url);
    EXPECT_EQ("http://example.com/{quadkey}.png", quadTile.tileData->urlTemplate);
    EXPECT_EQ(1, quadTile.tileData->pixelRatio);
    EXPECT_EQ(0, quadTile.tileData->x);
    EXPECT_EQ(0, quadTile.tileData->y);
    EXPECT_EQ(2, quadTile.tileData->z);

    quadTile = Resource::tile("http://example.com/{quadkey}.png", 2.0, 1, 1, 2, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, quadTile.kind);
    EXPECT_EQ("http://example.com/03.png", quadTile.url);
    EXPECT_EQ("http://example.com/{quadkey}.png", quadTile.tileData->urlTemplate);
    EXPECT_EQ(1, quadTile.tileData->pixelRatio);
    EXPECT_EQ(1, quadTile.tileData->x);
    EXPECT_EQ(1, quadTile.tileData->y);
    EXPECT_EQ(2, quadTile.tileData->z);

    quadTile = Resource::tile("http://example.com/{quadkey}.png", 2.0, 22914, 52870, 17, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, quadTile.kind);
    EXPECT_EQ("http://example.com/02301322130000230.png", quadTile.url);
    EXPECT_EQ("http://example.com/{quadkey}.png", quadTile.tileData->urlTemplate);
    EXPECT_EQ(1, quadTile.tileData->pixelRatio);
    EXPECT_EQ(22914, quadTile.tileData->x);
    EXPECT_EQ(52870, quadTile.tileData->y);
    EXPECT_EQ(17, quadTile.tileData->z);

    // Test case confirmed by quadkeytools package
    // https://bitbucket.org/steele/quadkeytools/src/master/test/quadkey.js?fileviewer=file-view-default#quadkey.js-57
    quadTile = Resource::tile("http://example.com/{quadkey}.png", 2.0, 29, 3, 6, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, quadTile.kind);
    EXPECT_EQ("http://example.com/011123.png", quadTile.url);
    EXPECT_EQ("http://example.com/{quadkey}.png", quadTile.tileData->urlTemplate);
    EXPECT_EQ(1, quadTile.tileData->pixelRatio);
    EXPECT_EQ(29, quadTile.tileData->x);
    EXPECT_EQ(3, quadTile.tileData->y);
    EXPECT_EQ(6, quadTile.tileData->z);

    Resource wmsTile = Resource::tile("http://example.com/?bbox={bbox-epsg-3857}", 2.0, 0, 0, 1, Tileset::Scheme::XYZ);
    EXPECT_EQ(Resource::Kind::Tile, wmsTile.kind);
    EXPECT_EQ("http://example.com/?bbox=-20037508.342789245,0,0,20037508.342789245", wmsTile.url);
    EXPECT_EQ("http://example.com/?bbox={bbox-epsg-3857}", wmsTile.tileData->urlTemplate);
    EXPECT_EQ(1, wmsTile.tileData->pixelRatio);
    EXPECT_EQ(0, wmsTile.tileData->x);
    EXPECT_EQ(0, wmsTile.tileData->y);
    EXPECT_EQ(1, wmsTile.tileData->z);

    Resource tmsTile = Resource::tile("http://example.com/{z}/{x}/{y}{ratio}.png", 1.0, 1, 2, 3, Tileset::Scheme::TMS);
    EXPECT_EQ(Resource::Kind::Tile, tmsTile.kind);
    EXPECT_EQ("http://example.com/3/1/5.png", tmsTile.url);
    EXPECT_EQ("http://example.com/{z}/{x}/{y}{ratio}.png", tmsTile.tileData->urlTemplate);
    EXPECT_EQ(1, tmsTile.tileData->pixelRatio);
    EXPECT_EQ(1, tmsTile.tileData->x);
    EXPECT_EQ(5, tmsTile.tileData->y);
    EXPECT_EQ(3, tmsTile.tileData->z);
}

TEST(Resource, Glyphs) {
    using namespace mbgl;
    Resource resource = Resource::glyphs("http://example.com/{fontstack}/{range}", {{"stack"}}, {0, 255});
    EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
    EXPECT_EQ("http://example.com/stack/0-255", resource.url);
}

TEST(Resource, SpriteImage) {
    using namespace mbgl;
    Resource resource = Resource::spriteImage("http://example.com/sprite", 2.0);
    EXPECT_EQ(Resource::Kind::SpriteImage, resource.kind);
    EXPECT_EQ("http://example.com/sprite@2x.png", resource.url);

    Resource paramResource = Resource::spriteImage("http://example.com/sprite?query=true", 2.0);
    EXPECT_EQ(Resource::Kind::SpriteImage, paramResource.kind);
    EXPECT_EQ("http://example.com/sprite@2x.png?query=true", paramResource.url);
}

TEST(Resource, Image) {
    using namespace mbgl;
    Resource resource = Resource::image("http://example.com/sprite.jpg");
    EXPECT_EQ(Resource::Kind::Image, resource.kind);
    EXPECT_EQ("http://example.com/sprite.jpg", resource.url);
}

TEST(Resource, SpriteJSON) {
    using namespace mbgl;
    Resource resource = Resource::spriteJSON("http://example.com/sprite", 2.0);
    EXPECT_EQ(Resource::Kind::SpriteJSON, resource.kind);
    EXPECT_EQ("http://example.com/sprite@2x.json", resource.url);

    Resource paramResource = Resource::spriteJSON("http://example.com/sprite?query=true", 2.0);
    EXPECT_EQ(Resource::Kind::SpriteJSON, paramResource.kind);
    EXPECT_EQ("http://example.com/sprite@2x.json?query=true", paramResource.url);
}
