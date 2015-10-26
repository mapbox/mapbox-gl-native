#include "../fixtures/util.hpp"

#include <mbgl/platform/log.hpp>
#include <mbgl/util/mapbox.hpp>
#include <regex>
#include <iostream>

using namespace mbgl;

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
    try {
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.png", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.png");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.png32", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.png32");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.png70", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.png70");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.png?access_token=foo", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.png?access_token=foo");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png", "http://path", SourceType::Raster), "http://path.png");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png", "", SourceType::Raster), "http://path.png");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.png", "mapbox://user.map", SourceType::Vector), "http://path.png/tile.png");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.pbf", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.pbf");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.pbf", "mapbox://user.map", SourceType::Vector), "http://path.png/tile.pbf");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.pbf?access_token=foo", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.pbf?access_token=foo");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.pbf?access_token=foo.png", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.pbf?access_token=foo.png");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.pbf?access_token=foo.png/bar", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.pbf?access_token=foo.png/bar");
        EXPECT_EQ(mbgl::util::mapbox::normalizeTileURL("http://path.png/tile.pbf?access_token=foo.png/bar.png", "mapbox://user.map", SourceType::Raster), "http://path.png/tile{ratio}.pbf?access_token=foo.png/bar.png");
    } catch (const std::regex_error& e) {
        const char *error = "unknown";
        switch (e.code()) {
            case std::regex_constants::error_collate:
                error = "error_collate"; break;
            case std::regex_constants::error_ctype:
                error = "error_ctype"; break;
            case std::regex_constants::error_escape:
                error = "error_escape"; break;
            case std::regex_constants::error_backref:
                error = "error_backref"; break;
            case std::regex_constants::error_paren:
                error = "error_paren"; break;
            case std::regex_constants::error_brace:
                error = "error_brace"; break;
            case std::regex_constants::error_badbrace:
                error = "error_badbrace"; break;
            case std::regex_constants::error_range:
                error = "error_range"; break;
            case std::regex_constants::error_space:
                error = "error_space"; break;
            case std::regex_constants::error_badrepeat:
                error = "error_badrepeat"; break;
            case std::regex_constants::error_complexity:
                error = "error_complexity"; break;
            case std::regex_constants::error_stack:
                error = "error_stack"; break;
            default:
                break;
        }
        mbgl::Log::Error(mbgl::Event::General, "regex_error caught: %s - %s (%d)", e.what(), error, e.code());
        throw e;
    }
}

TEST(Mapbox, CanonicalURL) {
    using mbgl::util::mapbox::canonicalURL;
    EXPECT_EQ(
        canonicalURL("https://a.tiles.mapbox.com/v4/"
                     "mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/"
                     "10744.vector.pbf?access_token=pk.kAeslEm93Sjf3mXk."
                     "vbiF02XnvkPkzlFhGSn2iIm6De3Cxsk5tmips2tvkG8sF"),
        "mapbox://v4/mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/10744.vector.pbf");

    EXPECT_EQ(
        canonicalURL("http://a.tiles.mapbox.com/v4/"
                     "mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/"
                     "10744.vector.pbf?access_token=pk.kAeslEm93Sjf3mXk."
                     "vbiF02XnvkPkzlFhGSn2iIm6De3Cxsk5tmips2tvkG8sF"),
        "mapbox://v4/mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/10744.vector.pbf");

    EXPECT_EQ(
        canonicalURL("https://b.tiles.mapbox.com/v4/"
                     "mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/"
                     "10744.vector.pbf?access_token=pk.kAeslEm93Sjf3mXk."
                     "vbiF02XnvkPkzlFhGSn2iIm6De3Cxsk5tmips2tvkG8sF"),
        "mapbox://v4/mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/10744.vector.pbf");

    EXPECT_EQ(
        canonicalURL("http://c.tiles.mapbox.com/v4/"
                     "mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/"
                     "10744.vector.pbf?access_token=pk.kAeslEm93Sjf3mXk."
                     "vbiF02XnvkPkzlFhGSn2iIm6De3Cxsk5tmips2tvkG8sF"),
        "mapbox://v4/mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/10744.vector.pbf");

    EXPECT_EQ(
        canonicalURL("https://api.mapbox.com/v4/"
                     "mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/"
                     "10744.vector.pbf?access_token=pk.kAeslEm93Sjf3mXk."
                     "vbiF02XnvkPkzlFhGSn2iIm6De3Cxsk5tmips2tvkG8sF"),
        "mapbox://v4/mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/10744.vector.pbf");

    EXPECT_EQ(
        canonicalURL("http://api.mapbox.com/v4/"
                     "mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/"
                     "10744.vector.pbf"),
        "mapbox://v4/mapbox.mapbox-terrain-v2,mapbox.mapbox-streets-v6/15/17599/10744.vector.pbf");

    EXPECT_EQ(canonicalURL("https://api.mapbox.com/fonts/v1/mapbox/"
                           "DIN%20Offc%20Pro%20Italic%2cArial%20Unicode%20MS%20Regular/"
                           "0-255.pbf?access_token=pk.kAeslEm93Sjf3mXk."
                           "vbiF02XnvkPkzlFhGSn2iIm6De3Cxsk5tmips2tvkG8sF"),
              "mapbox://fonts/v1/mapbox/DIN%20Offc%20Pro%20Italic%2cArial%20Unicode%20MS%20Regular/"
              "0-255.pbf");

    EXPECT_EQ(canonicalURL("https://api.mapbox.com/styles/v1/mapbox/streets-v8/"
                           "sprite.json?access_token=pk.kAeslEm93Sjf3mXk."
                           "vbiF02XnvkPkzlFhGSn2iIm6De3Cxsk5tmips2tvkG8sF"),
              "mapbox://styles/v1/mapbox/streets-v8/sprite.json");
}
