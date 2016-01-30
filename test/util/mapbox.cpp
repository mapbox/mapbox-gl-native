#include "../fixtures/util.hpp"

#include <mbgl/platform/log.hpp>
#include <mbgl/util/mapbox.hpp>
#include <regex>
#include <iostream>

using namespace mbgl;

// TODO: correct all EXPECT_EQ(actual, expected) to EXPECT_EQ(expected, actual)

TEST(Mapbox, SourceURL) {
    EXPECT_EQ(mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", "key"), "https://api.mapbox.com/v4/user.map.json?access_token=key&secure");
    EXPECT_EQ(mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", "token"), "https://api.mapbox.com/v4/user.map.json?access_token=token&secure");
    EXPECT_THROW(mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", ""), std::runtime_error);
}

TEST(Mapbox, GlyphsURL) {
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("mapbox://fonts/boxmap/Comic%20Sans/0-255.pbf", "key"), "https://api.mapbox.com/fonts/v1/boxmap/Comic%20Sans/0-255.pbf?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("mapbox://fonts/boxmap/{fontstack}/{range}.pbf", "key"), "https://api.mapbox.com/fonts/v1/boxmap/{fontstack}/{range}.pbf?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("http://path", "key"), "http://path");
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("mapbox://path", "key"), "mapbox://path");
}

TEST(Mapbox, StyleURL) {
    EXPECT_EQ(mbgl::util::mapbox::normalizeStyleURL("mapbox://foo", "key"), "mapbox://foo");
    EXPECT_EQ(mbgl::util::mapbox::normalizeStyleURL("mapbox://styles/user/style", "key"), "https://api.mapbox.com/styles/v1/user/style?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeStyleURL("mapbox://styles/user/style/draft", "key"), "https://api.mapbox.com/styles/v1/user/style/draft?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeStyleURL("http://path", "key"), "http://path");
}

TEST(Mapbox, SpriteURL) {
    EXPECT_EQ(mbgl::util::mapbox::normalizeSpriteURL("map/box/sprites@2x.json", "key"), "map/box/sprites@2x.json");
    EXPECT_EQ(mbgl::util::mapbox::normalizeSpriteURL("mapbox://foo", "key"), "mapbox://foo");
    EXPECT_EQ(mbgl::util::mapbox::normalizeSpriteURL("mapbox://sprites/mapbox/streets-v8.json", "key"), "https://api.mapbox.com/styles/v1/mapbox/streets-v8/sprite.json?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeSpriteURL("mapbox://sprites/mapbox/streets-v8@2x.png", "key"), "https://api.mapbox.com/styles/v1/mapbox/streets-v8/sprite@2x.png?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeSpriteURL("mapbox://sprites/mapbox/streets-v8/draft@2x.png", "key"), "https://api.mapbox.com/styles/v1/mapbox/streets-v8/draft/sprite@2x.png?access_token=key");
}

TEST(Mapbox, TileURL) {
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0.pbf?access_token=key",
        mbgl::util::mapbox::normalizeTileURL("mapbox://tiles/a.b/0/0/0.pbf", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0.png?access_token=key",
        mbgl::util::mapbox::normalizeTileURL("mapbox://tiles/a.b/0/0/0.png", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0@2x.webp?access_token=key",
        mbgl::util::mapbox::normalizeTileURL("mapbox://tiles/a.b/0/0/0@2x.webp", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b,c.d/0/0/0.pbf?access_token=key",
        mbgl::util::mapbox::normalizeTileURL("mapbox://tiles/a.b,c.d/0/0/0.pbf", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeSpriteURL("http://path", "key"));
}

TEST(Mapbox, CanonicalURL) {
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://a.tiles.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf", SourceType::Vector));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://b.tiles.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf", SourceType::Vector));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf", SourceType::Vector));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf?access_token=key", SourceType::Vector));
    EXPECT_EQ(
        "mapbox://tiles/a.b,c.d/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b,c.d/{z}/{x}/{y}.vector.pbf?access_token=key", SourceType::Vector));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.jpg",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.jpg?access_token=key", SourceType::Raster));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.jpg70",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.jpg70?access_token=key", SourceType::Raster));

#if defined(__ANDROID__) || defined(__APPLE__)
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.png",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png", SourceType::Raster));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.png",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png?access_token=key", SourceType::Raster));
#else
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.webp",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png", SourceType::Raster));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.webp",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png?access_token=key", SourceType::Raster));
#endif // defined(__ANDROID__) || defined(__APPLE__)

    // We don't ever expect to see these inputs, but be safe anyway.
    EXPECT_EQ(
        "",
        mbgl::util::mapbox::canonicalizeTileURL("", SourceType::Raster));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::canonicalizeTileURL("http://path", SourceType::Raster));
    EXPECT_EQ(
        "http://api.mapbox.com/v4/",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/", SourceType::Raster));
    EXPECT_EQ(
        "http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.", SourceType::Raster));
    EXPECT_EQ(
        "http://api.mapbox.com/v4/a.b/{z}/{x}/{y}/.",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}/.", SourceType::Raster));
}
