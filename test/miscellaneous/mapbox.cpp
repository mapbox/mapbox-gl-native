#include "../fixtures/util.hpp"

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
    EXPECT_EQ(mbgl::util::mapbox::normalizeGlyphsURL("http://path", "key"), "http://path");
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
        std::cout << "regex_error caught: " << e.what() << '\n';
        std::cout << e.code() << '\n';
        switch (e.code()) {
            case std::regex_constants::error_collate:
                std::cout << "error_collate" << '\n'; break;
            case std::regex_constants::error_ctype:
                std::cout << "error_ctype" << '\n'; break;
            case std::regex_constants::error_escape:
                std::cout << "error_escape" << '\n'; break;
            case std::regex_constants::error_backref:
                std::cout << "error_backref" << '\n'; break;
            case std::regex_constants::error_paren:
                std::cout << "error_paren" << '\n'; break;
            case std::regex_constants::error_brace:
                std::cout << "error_brace" << '\n'; break;
            case std::regex_constants::error_badbrace:
                std::cout << "error_badbrace" << '\n'; break;
            case std::regex_constants::error_range:
                std::cout << "error_range" << '\n'; break;
            case std::regex_constants::error_space:
                std::cout << "error_space" << '\n'; break;
            case std::regex_constants::error_badrepeat:
                std::cout << "error_badrepeat" << '\n'; break;
            case std::regex_constants::error_complexity:
                std::cout << "error_complexity" << '\n'; break;
            case std::regex_constants::error_stack:
                std::cout << "error_stack" << '\n'; break;
                
            default:
                break;
        }
        throw e;
    }
}
