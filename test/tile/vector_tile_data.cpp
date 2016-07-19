#include <mbgl/test/util.hpp>

#include <mbgl/tile/vector_tile_data.hpp>
#include <mbgl/util/io.hpp>

#include <iostream>
#include <fstream>

#include <dirent.h>

using namespace mbgl;

typedef std::pair<uint32_t, std::string> Message;
typedef std::vector<Message> Messages;

class TileParserTest : public ::testing::TestWithParam<std::string> {};

TEST_P(TileParserTest, ParseTile) {
    const std::string filename = std::string("test/fixtures/tile_parser/") + GetParam();

    VectorTileData tile(std::make_shared<const std::string>(util::read_file(filename)));
    tile.getLayer("foo");
}

INSTANTIATE_TEST_CASE_P(TileParser, TileParserTest, ::testing::ValuesIn([] {
    std::vector<std::string> names;

    const std::string tile_directory = "test/fixtures/tile_parser";
    DIR *dir = opendir(tile_directory.c_str());
    if (dir != nullptr) {
        for (dirent *dp = nullptr; (dp = readdir(dir)) != nullptr;) {
            names.push_back(dp->d_name);
        }
        closedir(dir);
    }

    EXPECT_GT(names.size(), 0u);
    return names;
}()));
