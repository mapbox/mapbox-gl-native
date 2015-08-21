#include "../fixtures/util.hpp"

#include <mbgl/platform/log.hpp>
#include <mbgl/util/mapbox.hpp>
#include <regex>
#include <iostream>

using namespace mbgl;

TEST(Mapbox, SourceURL) {
    EXPECT_EQ(mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", "key"), "https://api.tiles.mapbox.com/v4/user.map.json?access_token=key&secure");
    EXPECT_EQ(mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", "token"), "https://api.tiles.mapbox.com/v4/user.map.json?access_token=token&secure");
    EXPECT_THROW(mbgl::util::mapbox::normalizeSourceURL("mapbox://user.map", ""), std::runtime_error);
}

TEST(Mapbox, GlyphsURL) {
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("mapbox://fontstack/{fontstack}/{range}.pbf", "key"), "https://api.tiles.mapbox.com/v4/fontstack/{fontstack}/{range}.pbf?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("mapbox://fonts/v1/user/{fontstack}/{range}.pbf", "key"), "https://api.tiles.mapbox.com/fonts/v1/user/{fontstack}/{range}.pbf?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("http://path", "key"), "http://path");
}

TEST(Mapbox, StyleURL) {
    EXPECT_EQ(mbgl::util::mapbox::normalizeStyleURL("mapbox://user.style", "key"), "https://api.tiles.mapbox.com/styles/v1/user/user.style?access_token=key");
    EXPECT_EQ(mbgl::util::mapbox::normalizeStyleURL("http://path", "key"), "http://path");
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
