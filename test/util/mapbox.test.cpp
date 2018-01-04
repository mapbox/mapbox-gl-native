#include <mbgl/test/util.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/tileset.hpp>
#include <stdexcept>

using namespace mbgl;
using SourceType = mbgl::style::SourceType;

TEST(Mapbox, SourceURL) {
    EXPECT_EQ(
        "https://api.mapbox.com/v4/user.map.json?access_token=key&secure",
        mbgl::util::mapbox::normalizeSourceURL(util::API_BASE_URL, "mapbox://user.map", "key"));
    EXPECT_EQ(
        "https://api.example.com/v4/user.map.json?access_token=key&secure",
        mbgl::util::mapbox::normalizeSourceURL("https://api.example.com", "mapbox://user.map", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/user.map.json?access_token=key&secure&style=mapbox://styles/mapbox/streets-v9@0",
        mbgl::util::mapbox::normalizeSourceURL(util::API_BASE_URL, "mapbox://user.map?style=mapbox://styles/mapbox/streets-v9@0", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/user.map.json?access_token=key&secure",
        mbgl::util::mapbox::normalizeSourceURL(util::API_BASE_URL, "mapbox://user.map?", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeSourceURL(util::API_BASE_URL, "http://path", "key"));
    EXPECT_THROW(
        mbgl::util::mapbox::normalizeSourceURL(util::API_BASE_URL, "mapbox://user.map", ""),
        std::runtime_error);
}

TEST(Mapbox, GlyphsURL) {
    EXPECT_EQ(
        "https://api.mapbox.com/fonts/v1/boxmap/Comic%20Sans/0-255.pbf?access_token=key",
        mbgl::util::mapbox::normalizeGlyphsURL(util::API_BASE_URL, "mapbox://fonts/boxmap/Comic%20Sans/0-255.pbf", "key"));
    EXPECT_EQ(
        "https://api.example.com/fonts/v1/boxmap/Comic%20Sans/0-255.pbf?access_token=key",
         mbgl::util::mapbox::normalizeGlyphsURL("https://api.example.com", "mapbox://fonts/boxmap/Comic%20Sans/0-255.pbf", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/fonts/v1/boxmap/{fontstack}/{range}.pbf?access_token=key",
        mbgl::util::mapbox::normalizeGlyphsURL(util::API_BASE_URL, "mapbox://fonts/boxmap/{fontstack}/{range}.pbf", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeGlyphsURL(util::API_BASE_URL, "http://path", "key"));
    EXPECT_EQ(
        "mapbox://path",
        mbgl::util::mapbox::normalizeGlyphsURL(util::API_BASE_URL, "mapbox://path", "key"));
}

TEST(Mapbox, StyleURL) {
    EXPECT_EQ(
        "mapbox://foo",
        mbgl::util::mapbox::normalizeStyleURL(util::API_BASE_URL, "mapbox://foo", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/user/style?access_token=key",
        mbgl::util::mapbox::normalizeStyleURL(util::API_BASE_URL, "mapbox://styles/user/style", "key"));
    EXPECT_EQ(
        "https://api.example.com/styles/v1/user/style?access_token=key",
        mbgl::util::mapbox::normalizeStyleURL("https://api.example.com", "mapbox://styles/user/style", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/user/style/draft?access_token=key",
        mbgl::util::mapbox::normalizeStyleURL(util::API_BASE_URL, "mapbox://styles/user/style/draft", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/user/style?access_token=key&shave=true",
        mbgl::util::mapbox::normalizeStyleURL(util::API_BASE_URL, "mapbox://styles/user/style?shave=true", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/user/style?access_token=key",
        mbgl::util::mapbox::normalizeStyleURL(util::API_BASE_URL, "mapbox://styles/user/style?", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeStyleURL(util::API_BASE_URL, "http://path", "key"));
}

TEST(Mapbox, SpriteURL) {
    EXPECT_EQ(
        "map/box/sprites@2x.json",
        mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL, "map/box/sprites@2x.json", "key"));
    EXPECT_EQ(
        "mapbox://foo",
        mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL, "mapbox://foo", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/mapbox/streets-v8/sprite.json?access_token=key",
        mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL, "mapbox://sprites/mapbox/streets-v8.json", "key"));
    EXPECT_EQ(
        "https://api.example.com/styles/v1/mapbox/streets-v8/sprite.json?access_token=key",
        mbgl::util::mapbox::normalizeSpriteURL("https://api.example.com", "mapbox://sprites/mapbox/streets-v8.json", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/mapbox/streets-v8/sprite@2x.png?access_token=key",
        mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL, "mapbox://sprites/mapbox/streets-v8@2x.png", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/mapbox/streets-v8/draft/sprite@2x.png?access_token=key",
        mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL, "mapbox://sprites/mapbox/streets-v8/draft@2x.png", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/styles/v1/mapbox/streets-v10/sprite?access_token=key&fresh=true.png",
        mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL,
            "mapbox://sprites/mapbox/streets-v10?fresh=true.png",
            "key"));
    EXPECT_EQ("mapbox://////", mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL, "mapbox://////", "key"));
}

TEST(Mapbox, TileURL) {
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0.pbf?access_token=key",
        mbgl::util::mapbox::normalizeTileURL(util::API_BASE_URL, "mapbox://tiles/a.b/0/0/0.pbf", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0.pbf?access_token=key&style=mapbox://styles/mapbox/streets-v9@0",
        mbgl::util::mapbox::normalizeTileURL(util::API_BASE_URL, "mapbox://tiles/a.b/0/0/0.pbf?style=mapbox://styles/mapbox/streets-v9@0", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0.pbf?access_token=key",
        mbgl::util::mapbox::normalizeTileURL(util::API_BASE_URL, "mapbox://tiles/a.b/0/0/0.pbf?", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0.png?access_token=key",
        mbgl::util::mapbox::normalizeTileURL(util::API_BASE_URL, "mapbox://tiles/a.b/0/0/0.png", "key"));
    EXPECT_EQ(
        "https://api.example.com/v4/a.b/0/0/0.png?access_token=key",
        mbgl::util::mapbox::normalizeTileURL("https://api.example.com", "mapbox://tiles/a.b/0/0/0.png", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b/0/0/0@2x.webp?access_token=key",
        mbgl::util::mapbox::normalizeTileURL(util::API_BASE_URL, "mapbox://tiles/a.b/0/0/0@2x.webp", "key"));
    EXPECT_EQ(
        "https://api.mapbox.com/v4/a.b,c.d/0/0/0.pbf?access_token=key",
        mbgl::util::mapbox::normalizeTileURL(util::API_BASE_URL, "mapbox://tiles/a.b,c.d/0/0/0.pbf", "key"));
    EXPECT_EQ(
        "http://path",
        mbgl::util::mapbox::normalizeSpriteURL(util::API_BASE_URL, "http://path", "key"));
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
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("https://api.mapbox.cn/v4/a.b/{z}/{x}/{y}.vector.pbf?access_token=key", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b,c.d/{z}/{x}/{y}.vector.pbf",
        mbgl::util::mapbox::canonicalizeTileURL("http://api.mapbox.com/v4/a.b,c.d/{z}/{x}/{y}.vector.pbf?access_token=key", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf?custom=parameter",
        mbgl::util::mapbox::canonicalizeTileURL("http://a.tiles.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf?access_token=key&custom=parameter", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf?custom=parameter",
        mbgl::util::mapbox::canonicalizeTileURL("http://a.tiles.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf?custom=parameter&access_token=key", SourceType::Vector, 512));
    EXPECT_EQ(
        "mapbox://tiles/a.b/{z}/{x}/{y}.vector.pbf?custom=parameter&second=param",
        mbgl::util::mapbox::canonicalizeTileURL("http://a.tiles.mapbox.com/v4/a.b/{z}/{x}/{y}.vector.pbf?custom=parameter&access_token=key&second=param", SourceType::Vector, 512));
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

TEST(Mapbox, CanonicalizeRasterTileset) {
    mbgl::Tileset tileset;
    tileset.tiles = {
        "http://a.tiles.mapbox.com/v4/mapbox.satellite/{z}/{x}/{y}.png?access_token=key"
    };

    mbgl::util::mapbox::canonicalizeTileset(tileset, "mapbox://mapbox.satellite", SourceType::Raster, 256);

#if !defined(__ANDROID__) && !defined(__APPLE__)
    EXPECT_EQ("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.webp", tileset.tiles[0]);
#else
    EXPECT_EQ("mapbox://tiles/mapbox.satellite/{z}/{x}/{y}{ratio}.png", tileset.tiles[0]);
#endif
}

TEST(Mapbox, CanonicalizeVectorTileset) {
    mbgl::Tileset tileset;
    tileset.tiles = {
        "http://a.tiles.mapbox.com/v4/mapbox.streets/{z}/{x}/{y}.vector.pbf?access_token=key"
    };

    mbgl::util::mapbox::canonicalizeTileset(tileset, "mapbox://mapbox.streets", SourceType::Vector, 512);

    EXPECT_EQ("mapbox://tiles/mapbox.streets/{z}/{x}/{y}.vector.pbf", tileset.tiles[0]);
}
