#include <iostream>
#include "../fixtures/util.hpp"

#include <algorithm>

#include <mbgl/util/clip_id.hpp>
#include <mbgl/map/tile.hpp>
#include <mbgl/util/std.hpp>

using namespace mbgl;

template <typename T> void generate(const T &sources) {
    ClipIDGenerator generator;

    for (size_t j = 0; j < sources.size(); j++) {
        std::forward_list<Tile *> tile_ptrs;
        std::transform(sources[j].begin(), sources[j].end(), std::front_inserter(tile_ptrs), [](const std::shared_ptr<Tile> &tile) { return tile.get(); });
        generator.update(tile_ptrs);
    }
}

template <typename T> void print(const T &sources) {
    for (size_t j = 0; j < sources.size(); j++) {
        for (size_t i = 0; i < sources[j].size(); i++) {
            std::cout << "    ASSERT_EQ(ClipID(\"" << sources[j][i]->clip.mask << "\", \"" << sources[j][i]->clip.reference << "\"), sources[" << j << "][" << i << "]->clip);\n";
        }
    }
}

TEST(ClipIDs, ParentAndFourChildren) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 1, 0, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 0, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 1, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 0, 0, 0 }, 512.0f),
        },
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][4]->clip);
}

TEST(ClipIDs, ParentAndFourChildrenNegative) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 1, -2, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, -2, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, -1, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, -1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 0, -1, 0 }, 512.0f),
        },
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][4]->clip);
}

TEST(ClipIDs, NegativeParentAndMissingLevel) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 1, -1, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, -1, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, -2, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, -1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, -2, 0 }, 512.0f),
        },
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][4]->clip);
}


TEST(ClipIDs, SevenOnSameLevel) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 2, 0, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 0, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 0, 2 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 1, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 1, 2 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 2, 0 }, 512.0f),
        },
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][4]->clip);
    ASSERT_EQ(ClipID("00000111", "00000110"), sources[0][5]->clip);
    ASSERT_EQ(ClipID("00000111", "00000111"), sources[0][6]->clip);
}

TEST(ClipIDs, MultipleLevels) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 2, 0, 0 }, 512.0f),
                std::make_shared<Tile>(TileID { 3, 0, 0 }, 512.0f),
                std::make_shared<Tile>(TileID { 3, 0, 1 }, 512.0f),
                    std::make_shared<Tile>(TileID { 4, 0, 2 }, 512.0f),
                    std::make_shared<Tile>(TileID { 4, 1, 2 }, 512.0f),
                    std::make_shared<Tile>(TileID { 4, 0, 3 }, 512.0f),
                    std::make_shared<Tile>(TileID { 4, 1, 3 }, 512.0f),
                std::make_shared<Tile>(TileID { 3, 1, 0 }, 512.0f),
                std::make_shared<Tile>(TileID { 3, 1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 1, 0 }, 512.0f),
                std::make_shared<Tile>(TileID { 3, 2, 0 }, 512.0f),
                std::make_shared<Tile>(TileID { 3, 2, 1 }, 512.0f),
        },
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00001111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00001111", "00000011"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00001111", "00000100"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00001111", "00001001"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00001111", "00001011"), sources[0][4]->clip);
    ASSERT_EQ(ClipID("00001111", "00001010"), sources[0][5]->clip);
    ASSERT_EQ(ClipID("00001111", "00001100"), sources[0][6]->clip);
    ASSERT_EQ(ClipID("00001111", "00000101"), sources[0][7]->clip);
    ASSERT_EQ(ClipID("00001111", "00000110"), sources[0][8]->clip);
    ASSERT_EQ(ClipID("00001111", "00000010"), sources[0][9]->clip);
    ASSERT_EQ(ClipID("00001111", "00000111"), sources[0][10]->clip);
    ASSERT_EQ(ClipID("00001111", "00001000"), sources[0][11]->clip);
}


TEST(ClipIDs, Bug206) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 10, 162, 395 }, 512.0f),
            std::make_shared<Tile>(TileID { 10, 162, 396 }, 512.0f),
            std::make_shared<Tile>(TileID { 10, 163, 395 }, 512.0f),
                std::make_shared<Tile>(TileID { 11, 326, 791 }, 512.0f),
                std::make_shared<Tile>(TileID { 12, 654, 1582 }, 512.0f),
                std::make_shared<Tile>(TileID { 12, 654, 1583 }, 512.0f),
                std::make_shared<Tile>(TileID { 12, 655, 1582 }, 512.0f),
                std::make_shared<Tile>(TileID { 12, 655, 1583 }, 512.0f),
            std::make_shared<Tile>(TileID { 10, 163, 396 }, 512.0f),
            std::make_shared<Tile>(TileID { 10, 164, 395 }, 512.0f),
            std::make_shared<Tile>(TileID { 10, 164, 396 }, 512.0f),
        },
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00001111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00001111", "00000010"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00001111", "00000011"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00001111", "00000111"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00001111", "00001000"), sources[0][4]->clip);
    ASSERT_EQ(ClipID("00001111", "00001001"), sources[0][5]->clip);
    ASSERT_EQ(ClipID("00001111", "00001010"), sources[0][6]->clip);
    ASSERT_EQ(ClipID("00001111", "00001011"), sources[0][7]->clip);
    ASSERT_EQ(ClipID("00001111", "00000100"), sources[0][8]->clip);
    ASSERT_EQ(ClipID("00001111", "00000101"), sources[0][9]->clip);
    ASSERT_EQ(ClipID("00001111", "00000110"), sources[0][10]->clip);
}


TEST(ClipIDs, MultipleSources) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 0, 0, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 2, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 2, 2 }, 512.0f),
        },
        {
            std::make_shared<Tile>(TileID { 0, 0, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 2, 2 }, 512.0f),
        },
        {
            std::make_shared<Tile>(TileID { 1, 0, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 0, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 1, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 1, 1, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 1, 1 }, 512.0f),
        },
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00011000", "00001000"), sources[1][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000010"), sources[1][1]->clip);
    ASSERT_EQ(ClipID("00011000", "00010000"), sources[1][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[1][3]->clip);
    ASSERT_EQ(ClipID("11100000", "00100000"), sources[2][0]->clip);
    ASSERT_EQ(ClipID("11100000", "01000000"), sources[2][1]->clip);
    ASSERT_EQ(ClipID("11100000", "01100000"), sources[2][2]->clip);
    ASSERT_EQ(ClipID("11100000", "10000000"), sources[2][3]->clip);
    ASSERT_EQ(ClipID("00011000", "00010000"), sources[2][4]->clip);
}


TEST(ClipIDs, DuplicateIDs) {
        const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 2, 0, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 0, 1 }, 512.0f),
        },
        {
            std::make_shared<Tile>(TileID { 2, 0, 0 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 0, 1 }, 512.0f),
            std::make_shared<Tile>(TileID { 2, 0, 1 }, 512.0f),
        }
    };

    generate(sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000011", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000011", "00000010"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000011", "00000001"), sources[1][0]->clip);
    ASSERT_EQ(ClipID("00000011", "00000010"), sources[1][1]->clip);
    ASSERT_EQ(ClipID("00000011", "00000010"), sources[1][2]->clip);
}
