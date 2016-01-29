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
    Resource resource = Resource::tile("http://example.com/{z}/{x}/{y}{ratio}.png", 2.0, 1, 2, 3);
    EXPECT_EQ(Resource::Kind::Tile, resource.kind);
    EXPECT_EQ("http://example.com/3/1/2@2x.png", resource.url);
    EXPECT_EQ("http://example.com/{z}/{x}/{y}{ratio}.png", resource.tileData->urlTemplate);
    EXPECT_EQ(2.0, resource.tileData->pixelRatio);
    EXPECT_EQ(1, resource.tileData->x);
    EXPECT_EQ(2, resource.tileData->y);
    EXPECT_EQ(3, resource.tileData->z);
}

TEST(Resource, Glyphs) {
    using namespace mbgl;
    Resource resource = Resource::glyphs("http://example.com/{fontstack}/{range}", "stack", {0, 255});
    EXPECT_EQ(Resource::Kind::Glyphs, resource.kind);
    EXPECT_EQ("http://example.com/stack/0-255", resource.url);
}

TEST(Resource, SpriteImage) {
    using namespace mbgl;
    Resource resource = Resource::spriteImage("http://example.com/sprite", 2.0);
    EXPECT_EQ(Resource::Kind::SpriteImage, resource.kind);
    EXPECT_EQ("http://example.com/sprite@2x.png", resource.url);
}

TEST(Resource, SpriteJSON) {
    using namespace mbgl;
    Resource resource = Resource::spriteJSON("http://example.com/sprite", 2.0);
    EXPECT_EQ(Resource::Kind::SpriteJSON, resource.kind);
    EXPECT_EQ("http://example.com/sprite@2x.json", resource.url);
}
