#include <mbgl/test/util.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/util/mapbox.hpp>
#include <regex>
#include <iostream>

using namespace mbgl;

TEST(Mapbox, SourceURL) {
    EXPECT_EQ(
        "https://api.mapbox.com/v4/user.map.json?access_token=key&secure",
        mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeSourceURL("http://path", "key"));
    EXPECT_THROW(
        mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", ""),
        std::runtime_error);
}

TEST(Mapbox, GlyphsURL) {
    EXPECT_EQ(
        "https://api.mapbox.com/fonts/v1/boxmap/Comic%20Sans/0-255.pbf?access_token=key",
        mbgl::util::mapbox::normalizeGlyphsURL("mapbox://fonts/boxmap/Comic%20Sans/0-255.pbf", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/fonts/v1/boxmap/{fontstack}/{range}.pbf?access_token=key",
        mbgl::util::mapbox::normalizeGlyphsURL("mapbox://fonts/boxmap/{fontstack}/{range}.pbf", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeGlyphsURL("http://path", "key"));
    EXPECT_EQ(
        "mapbox://path",
        mbgl::util::mapbox::normalizeGlyphsURL("mapbox://path", "key"));
}

TEST(Mapbox, StyleURL) {
    EXPECT_EQ(
        "mapbox://foo",
        mbgl::util::mapbox::normalizeStyleURL("mapbox://foo", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/user/style?access_token=key",
        mbgl::util::mapbox::normalizeStyleURL("mapbox://styles/user/style", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/user/style/draft?access_token=key",
        mbgl::util::mapbox::normalizeStyleURL("mapbox://styles/user/style/draft", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeStyleURL("http://path", "key"));
}

TEST(Mapbox, SpriteURL) {
    EXPECT_EQ(
        "map/box/sprites@2x.json",
        mbgl::util::mapbox::normalizeSpriteURL("map/box/sprites@2x.json", "key"));
    EXPECT_EQ(
        "mapbox://foo",
        mbgl::util::mapbox::normalizeSpriteURL("mapbox://foo", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/mapbox/streets-v8/sprite.json?access_token=key",
        mbgl::util::mapbox::normalizeSpriteURL("mapbox://sprites/mapbox/streets-v8.json", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/mapbox/streets-v8/sprite@2x.png?access_token=key",
        mbgl::util::mapbox::normalizeSpriteURL("mapbox://sprites/mapbox/streets-v8@2x.png", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/mapbox/streets-v8/draft/sprite@2x.png?access_token=key",
        mbgl::util::mapbox::normalizeSpriteURL("mapbox://sprites/mapbox/streets-v8/draft@2x.png", "key"));
    EXPECT_EQ(
        "mapbox://sprites/mapbox/streets-v9?fresh=true.png",
        mbgl::util::mapbox::normalizeSpriteURL(
            "mapbox://sprites/mapbox/streets-v9?fresh=true.png",
            "key"));
    EXPECT_EQ("mapbox://////", mbgl::util::mapbox::normalizeSpriteURL("mapbox://////", "key"));
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
        mbgl::util::mapbox::canonicalizeTileURL("http://a.tiles.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://b.tiles.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf?access_token=key", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b,c.d/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b,c.d/{z}/{x}/{y}.vector.pbf?access_token=key", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.jpg",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.jpg?access_token=key", SourceType::Raster, 256));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.jpg70",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.jpg70?access_token=key", SourceType::Raster, 256));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}@2x.jpg",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.jpg?access_token=key", SourceType::Raster, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}@2x.jpg70",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.jpg70?access_token=key", SourceType::Raster, 512));

#if defined(__ANDROID__) || defined(__APPLE__)
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.png",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png", SourceType::Raster, 256));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.png",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png?access_token=key", SourceType::Raster, 256));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}@2x.png",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png", SourceType::Raster, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}@2x.png",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png?access_token=key", SourceType::Raster, 512));
#else
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.webp",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png", SourceType::Raster, 256));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}{ratio}.webp",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png?access_token=key", SourceType::Raster, 256));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}@2x.webp",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png", SourceType::Raster, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}@2x.webp",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.png?access_token=key", SourceType::Raster, 512));
#endif // defined(__ANDROID__) || defined(__APPLE__)

    // We don't ever expect to see these inputs, but be safe anyway.
    EXPECT_EQ(
        "",
        mbgl::util::mapbox::canonicalizeTileURL("", SourceType::Raster, 256));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::canonicalizeTileURL("http://path", SourceType::Raster, 256));
    EXPECT_EQ(
        "http://api.mapbox.com/v4/",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/", SourceType::Raster, 256));
    EXPECT_EQ(
        "http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}.", SourceType::Raster, 256));
    EXPECT_EQ(
        "http://api.mapbox.com/v4/a.b/{z}/{x}/{y}/.",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b/{z}/{x}/{y}/.", SourceType::Raster, 256));
}
