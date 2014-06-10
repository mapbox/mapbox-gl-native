#include <iostream>
#include "gtest/gtest.h"

#include <algorithm>

#include <llmr/util/clip_ids.hpp>
#include <llmr/util/std.hpp>

using namespace llmr;

TEST(ClipIDs, ClipMasks) {
    ASSERT_EQ(0, clipMask[0]);
    ASSERT_EQ(0x80, clipMask[1]);
    ASSERT_EQ(0xC0, clipMask[2]);
    ASSERT_EQ(0xE0, clipMask[3]);
    ASSERT_EQ(0xF0, clipMask[4]);
    ASSERT_EQ(0xF8, clipMask[5]);
    ASSERT_EQ(0xFC, clipMask[6]);
    ASSERT_EQ(0xFE, clipMask[7]);
    ASSERT_EQ(0xFF, clipMask[8]);
}


TEST(ClipIDs, ParentAndFourChildren) {
    std::array<std::unique_ptr<Tile>, 5> tiles {{
        std::make_unique<Tile>(Tile::ID { 1, 0, 0 }), // 1/0/0: 11000000 (3)
        std::make_unique<Tile>(Tile::ID { 1, 0, 1 }), // 1/0/1: 11100000 (3)
        std::make_unique<Tile>(Tile::ID { 1, 1, 0 }), // 1/1/0: 10000000 (3)
        std::make_unique<Tile>(Tile::ID { 1, 1, 1 }), // 1/1/1: 10100000 (3)
        std::make_unique<Tile>(Tile::ID { 0, 0, 0 }), // 0/0/0: 10000000 (1)
    }};

    std::list<Tile *> tile_ptrs;
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tile_ptrs), [](std::unique_ptr<Tile> &tile) { return tile.get(); });

    updateClipIDs(tile_ptrs);

    // for (const auto &it : tiles) {
    //     std::cout << std::string(it->id) << ": " << it->clip.mask << " (" << (int)it->clip.length << ")" << std::endl;
    // }

    ASSERT_EQ(std::bitset<8>("11000000"), tiles[0]->clip.mask); ASSERT_EQ(3, tiles[0]->clip.length);
    ASSERT_EQ(std::bitset<8>("11100000"), tiles[1]->clip.mask); ASSERT_EQ(3, tiles[1]->clip.length);
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[2]->clip.mask); ASSERT_EQ(3, tiles[2]->clip.length);
    ASSERT_EQ(std::bitset<8>("10100000"), tiles[3]->clip.mask); ASSERT_EQ(3, tiles[3]->clip.length);
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[4]->clip.mask); ASSERT_EQ(1, tiles[4]->clip.length);
}

TEST(ClipIDs, ParentAndFourChildrenNegative) {
    std::array<std::unique_ptr<Tile>, 5> tiles {{
        std::make_unique<Tile>(Tile::ID { 1, -2, 0 }), // 1/0/0: 11000000 (3)
        std::make_unique<Tile>(Tile::ID { 1, -2, 1 }), // 1/0/1: 11100000 (3)
        std::make_unique<Tile>(Tile::ID { 1, -1, 0 }), // 1/1/0: 10000000 (3)
        std::make_unique<Tile>(Tile::ID { 1, -1, 1 }), // 1/1/1: 10100000 (3)
        std::make_unique<Tile>(Tile::ID { 0, -1, 0 }), // 0/0/0: 10000000 (1)
    }};

    std::list<Tile *> tile_ptrs;
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tile_ptrs), [](std::unique_ptr<Tile> &tile) { return tile.get(); });

    updateClipIDs(tile_ptrs);

    // for (const auto &it : tiles) {
    //     std::cout << std::string(it->id) << ": " << it->clip.mask << " (" << (int)it->clip.length << ")" << std::endl;
    // }

    ASSERT_EQ(std::bitset<8>("11000000"), tiles[0]->clip.mask); ASSERT_EQ(3, tiles[0]->clip.length);
    ASSERT_EQ(std::bitset<8>("11100000"), tiles[1]->clip.mask); ASSERT_EQ(3, tiles[1]->clip.length);
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[2]->clip.mask); ASSERT_EQ(3, tiles[2]->clip.length);
    ASSERT_EQ(std::bitset<8>("10100000"), tiles[3]->clip.mask); ASSERT_EQ(3, tiles[3]->clip.length);
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[4]->clip.mask); ASSERT_EQ(1, tiles[4]->clip.length);
}

TEST(ClipIDs, NegativeParentAndMissingLevel) {
    std::array<std::unique_ptr<Tile>, 5> tiles {{
        std::make_unique<Tile>(Tile::ID { 1, -1, 0 }), // 1/-1/0: 10000000 (1)
        std::make_unique<Tile>(Tile::ID { 2, -1, 0 }), // 2/-1/0: 10000000 (3)
        std::make_unique<Tile>(Tile::ID { 2, -2, 1 }), // 2/-2/1: 11100000 (3)
        std::make_unique<Tile>(Tile::ID { 2, -1, 1 }), // 2/-1/1: 10100000 (3)
        std::make_unique<Tile>(Tile::ID { 2, -2, 0 }), // 2/-2/0: 11000000 (3)
    }};

    std::list<Tile *> tile_ptrs;
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tile_ptrs), [](std::unique_ptr<Tile> &tile) { return tile.get(); });

    updateClipIDs(tile_ptrs);

    // for (const auto &it : tiles) {
    //     std::cout << std::string(it->id) << ": " << it->clip.mask << " (" << (int)it->clip.length << ")" << std::endl;
    // }

    ASSERT_EQ(std::bitset<8>("10000000"), tiles[0]->clip.mask); ASSERT_EQ(1, tiles[0]->clip.length);
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[1]->clip.mask); ASSERT_EQ(3, tiles[1]->clip.length);
    ASSERT_EQ(std::bitset<8>("11100000"), tiles[2]->clip.mask); ASSERT_EQ(3, tiles[2]->clip.length);
    ASSERT_EQ(std::bitset<8>("10100000"), tiles[3]->clip.mask); ASSERT_EQ(3, tiles[3]->clip.length);
    ASSERT_EQ(std::bitset<8>("11000000"), tiles[4]->clip.mask); ASSERT_EQ(3, tiles[4]->clip.length);
}


TEST(ClipIDs, SevenOnSameLevel) {
    std::array<std::unique_ptr<Tile>, 7> tiles {{
        std::make_unique<Tile>(Tile::ID { 2, 0, 0 }), // 2/0/0: 11000000 (3)
        std::make_unique<Tile>(Tile::ID { 2, 0, 1 }), // 2/0/1: 11100000 (3)
        std::make_unique<Tile>(Tile::ID { 2, 0, 2 }), // 2/0/2: 10000000 (3)
        std::make_unique<Tile>(Tile::ID { 2, 1, 0 }), // 2/1/0: 10100000 (3)
        std::make_unique<Tile>(Tile::ID { 2, 1, 1 }), // 2/1/1: 01000000 (3)
        std::make_unique<Tile>(Tile::ID { 2, 1, 2 }), // 2/1/2: 01100000 (3)
        std::make_unique<Tile>(Tile::ID { 2, 2, 0 }), // 2/2/0: 00100000 (3)
    }};

    std::list<Tile *> tile_ptrs;
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tile_ptrs), [](std::unique_ptr<Tile> &tile) { return tile.get(); });

    updateClipIDs(tile_ptrs);

    // for (const auto &it : tiles) {
    //     std::cout << std::string(it->id) << ": " << it->clip.mask << " (" << (int)it->clip.length << ")" << std::endl;
    // }

    ASSERT_EQ(std::bitset<8>("11000000"), tiles[0]->clip.mask); ASSERT_EQ(3, tiles[0]->clip.length);
    ASSERT_EQ(std::bitset<8>("11100000"), tiles[1]->clip.mask); ASSERT_EQ(3, tiles[1]->clip.length);
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[2]->clip.mask); ASSERT_EQ(3, tiles[2]->clip.length);
    ASSERT_EQ(std::bitset<8>("10100000"), tiles[3]->clip.mask); ASSERT_EQ(3, tiles[3]->clip.length);
    ASSERT_EQ(std::bitset<8>("01000000"), tiles[4]->clip.mask); ASSERT_EQ(3, tiles[4]->clip.length);
    ASSERT_EQ(std::bitset<8>("01100000"), tiles[5]->clip.mask); ASSERT_EQ(3, tiles[5]->clip.length);
    ASSERT_EQ(std::bitset<8>("00100000"), tiles[6]->clip.mask); ASSERT_EQ(3, tiles[6]->clip.length);
}

TEST(ClipIDs, MultipleLevels) {
    std::array<std::unique_ptr<Tile>, 12> tiles {{
        std::make_unique<Tile>(Tile::ID { 2, 0, 0 }),           // 2/0/0: 10000000 (1)
            std::make_unique<Tile>(Tile::ID { 3, 0, 0 }),       //     3/0/0: 10000000 (3)
            std::make_unique<Tile>(Tile::ID { 3, 0, 1 }),       //     3/0/1: 11100000 (3)
                std::make_unique<Tile>(Tile::ID { 4, 0, 2 }),   //         4/0/2: 11110000 (5)
                std::make_unique<Tile>(Tile::ID { 4, 1, 2 }),   //         4/0/3: 11111000 (5)
                std::make_unique<Tile>(Tile::ID { 4, 0, 3 }),   //         4/1/2: 11100000 (5)
                std::make_unique<Tile>(Tile::ID { 4, 1, 3 }),   //         4/1/3: 11101000 (5)
            std::make_unique<Tile>(Tile::ID { 3, 1, 0 }),       //     3/1/0: 10100000 (3)
            std::make_unique<Tile>(Tile::ID { 3, 1, 1 }),       //     3/1/1: 11000000 (3)
        std::make_unique<Tile>(Tile::ID { 2, 1, 0 }),           // 2/1/0: 01000000 (2)
            std::make_unique<Tile>(Tile::ID { 3, 2, 0 }),       //     3/2/0: 01010000 (4)
            std::make_unique<Tile>(Tile::ID { 3, 2, 1 }),       //     3/2/1: 01100000 (3)
    }};

    // Use a random order to verify that the result of this algorithm is independent of the
    std::random_shuffle(tiles.begin(), tiles.end());

    std::list<Tile *> tile_ptrs;
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tile_ptrs), [](std::unique_ptr<Tile> &tile) { return tile.get(); });

    updateClipIDs(tile_ptrs);

    // Sort them by tile ID so that we know what order we have to test in.
    std::sort(tiles.begin(), tiles.end(), [](const std::unique_ptr<Tile> &a, const std::unique_ptr<Tile> &b) {
        return a->id < b->id;
    });

    // for (const auto &it : tiles) {
    //     std::cout << std::string(it->id) << ": " << it->clip.mask << " (" << (int)it->clip.length << ")" << std::endl;
    // }

    ASSERT_EQ(std::string("2/0/0"), std::string(tiles[0]->id));
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[0]->clip.mask);
    ASSERT_EQ(1, tiles[0]->clip.length);

    ASSERT_EQ(std::string("2/1/0"), std::string(tiles[1]->id));
    ASSERT_EQ(std::bitset<8>("01000000"), tiles[1]->clip.mask);
    ASSERT_EQ(2, tiles[1]->clip.length);

    ASSERT_EQ(std::string("3/0/0"), std::string(tiles[2]->id));
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[2]->clip.mask);
    ASSERT_EQ(3, tiles[2]->clip.length);

    ASSERT_EQ(std::string("3/0/1"), std::string(tiles[3]->id));
    ASSERT_EQ(std::bitset<8>("11100000"), tiles[3]->clip.mask);
    ASSERT_EQ(3, tiles[3]->clip.length);

    ASSERT_EQ(std::string("3/1/0"), std::string(tiles[4]->id));
    ASSERT_EQ(std::bitset<8>("10100000"), tiles[4]->clip.mask);
    ASSERT_EQ(3, tiles[4]->clip.length);

    ASSERT_EQ(std::string("3/1/1"), std::string(tiles[5]->id));
    ASSERT_EQ(std::bitset<8>("11000000"), tiles[5]->clip.mask);
    ASSERT_EQ(3, tiles[5]->clip.length);

    ASSERT_EQ(std::string("3/2/0"), std::string(tiles[6]->id));
    ASSERT_EQ(std::bitset<8>("01010000"), tiles[6]->clip.mask);
    ASSERT_EQ(4, tiles[6]->clip.length);

    ASSERT_EQ(std::string("3/2/1"), std::string(tiles[7]->id));
    ASSERT_EQ(std::bitset<8>("01100000"), tiles[7]->clip.mask);
    ASSERT_EQ(3, tiles[7]->clip.length);

    ASSERT_EQ(std::string("4/0/2"), std::string(tiles[8]->id));
    ASSERT_EQ(std::bitset<8>("11110000"), tiles[8]->clip.mask);
    ASSERT_EQ(5, tiles[8]->clip.length);

    ASSERT_EQ(std::string("4/0/3"), std::string(tiles[9]->id));
    ASSERT_EQ(std::bitset<8>("11111000"), tiles[9]->clip.mask);
    ASSERT_EQ(5, tiles[9]->clip.length);

    ASSERT_EQ(std::string("4/1/2"), std::string(tiles[10]->id));
    ASSERT_EQ(std::bitset<8>("11100000"), tiles[10]->clip.mask);
    ASSERT_EQ(5, tiles[10]->clip.length);

    ASSERT_EQ(std::string("4/1/3"), std::string(tiles[11]->id));
    ASSERT_EQ(std::bitset<8>("11101000"), tiles[11]->clip.mask);
    ASSERT_EQ(5, tiles[11]->clip.length);
}


TEST(ClipIDs, Bug206) {
    std::array<std::unique_ptr<Tile>, 11> tiles {{
        std::make_unique<Tile>(Tile::ID { 10, 162, 395 }),      // 10/162/395: 10000000 (3)
        std::make_unique<Tile>(Tile::ID { 10, 162, 396 }),      // 10/162/396: 10100000 (3)
        std::make_unique<Tile>(Tile::ID { 10, 163, 395 }),      // 10/163/395: 11000000 (2)
            std::make_unique<Tile>(Tile::ID { 11, 326, 791 }),  //     11/326/791: 11100000 (4)
            std::make_unique<Tile>(Tile::ID { 12, 654, 1582 }), //     12/654/1582: 11001000 (5)
            std::make_unique<Tile>(Tile::ID { 12, 654, 1583 }), //     12/654/1583: 11010000 (4)
            std::make_unique<Tile>(Tile::ID { 12, 655, 1582 }), //     12/655/1582: 11110000 (5)
            std::make_unique<Tile>(Tile::ID { 12, 655, 1583 }), //     12/655/1583: 11111000 (5)
        std::make_unique<Tile>(Tile::ID { 10, 163, 396 }),      // 10/163/396: 01000000 (3)
        std::make_unique<Tile>(Tile::ID { 10, 164, 395 }),      // 10/164/395: 01100000 (3)
        std::make_unique<Tile>(Tile::ID { 10, 164, 396 }),      // 10/164/396: 00100000 (3)
    }};

    std::list<Tile *> tile_ptrs;
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tile_ptrs), [](std::unique_ptr<Tile> &tile) { return tile.get(); });

    updateClipIDs(tile_ptrs);

    // for (const auto &it : tiles) {
    //     std::cout << std::string(it->id) << ": " << it->clip.mask << " (" << (int)it->clip.length << ")" << std::endl;
    // }

    ASSERT_EQ(std::string("10/162/395"), std::string(tiles[0]->id));
    ASSERT_EQ(std::bitset<8>("10000000"), tiles[0]->clip.mask);
    ASSERT_EQ(3, tiles[0]->clip.length);

    ASSERT_EQ(std::string("10/162/396"), std::string(tiles[1]->id));
    ASSERT_EQ(std::bitset<8>("10100000"), tiles[1]->clip.mask);
    ASSERT_EQ(3, tiles[1]->clip.length);

    ASSERT_EQ(std::string("10/163/395"), std::string(tiles[2]->id));
    ASSERT_EQ(std::bitset<8>("11000000"), tiles[2]->clip.mask);
    ASSERT_EQ(2, tiles[2]->clip.length);

    ASSERT_EQ(std::string("11/326/791"), std::string(tiles[3]->id));
    ASSERT_EQ(std::bitset<8>("11100000"), tiles[3]->clip.mask);
    ASSERT_EQ(4, tiles[3]->clip.length);

    ASSERT_EQ(std::string("12/654/1582"), std::string(tiles[4]->id));
    ASSERT_EQ(std::bitset<8>("11001000"), tiles[4]->clip.mask);
    ASSERT_EQ(5, tiles[4]->clip.length);

    ASSERT_EQ(std::string("12/654/1583"), std::string(tiles[5]->id));
    ASSERT_EQ(std::bitset<8>("11010000"), tiles[5]->clip.mask);
    ASSERT_EQ(4, tiles[5]->clip.length);

    ASSERT_EQ(std::string("12/655/1582"), std::string(tiles[6]->id));
    ASSERT_EQ(std::bitset<8>("11110000"), tiles[6]->clip.mask);
    ASSERT_EQ(5, tiles[6]->clip.length);

    ASSERT_EQ(std::string("12/655/1583"), std::string(tiles[7]->id));
    ASSERT_EQ(std::bitset<8>("11111000"), tiles[7]->clip.mask);
    ASSERT_EQ(5, tiles[7]->clip.length);

    ASSERT_EQ(std::string("10/163/396"), std::string(tiles[8]->id));
    ASSERT_EQ(std::bitset<8>("01000000"), tiles[8]->clip.mask);
    ASSERT_EQ(3, tiles[8]->clip.length);

    ASSERT_EQ(std::string("10/164/395"), std::string(tiles[9]->id));
    ASSERT_EQ(std::bitset<8>("01100000"), tiles[9]->clip.mask);
    ASSERT_EQ(3, tiles[9]->clip.length);

    ASSERT_EQ(std::string("10/164/396"), std::string(tiles[10]->id));
    ASSERT_EQ(std::bitset<8>("00100000"), tiles[10]->clip.mask);
    ASSERT_EQ(3, tiles[10]->clip.length);

}

TEST(ClipIDs, DuplicateIDs) {

    std::forward_list<Tile::ID> tiles {{
        Tile::ID { 2, 0, 0 },
        Tile::ID { 2, 0, 1 },
        Tile::ID { 2, 0, 0 },
        Tile::ID { 2, 0, 1 },
        Tile::ID { 2, 0, 1 },
    }};

    std::map<Tile::ID, ClipID> mapping = computeClipIDs(tiles);

    ASSERT_EQ(2ull, mapping.size());

    // for (const auto &it : mapping) {
    //     std::cout << std::string(it.first) << "(" << it.first << ")" << ": " << it.second.mask << " (" << (int)it.second.length << ")" << std::endl;
    // }

    ASSERT_EQ(std::bitset<8>("01000000"), mapping[Tile::ID(2, 0, 0)].mask);
    ASSERT_EQ(2, mapping[Tile::ID(2, 0, 0)].length);

    ASSERT_EQ(std::bitset<8>("10000000"), mapping[Tile::ID(2, 0, 1)].mask);
    ASSERT_EQ(1, mapping[Tile::ID(2, 0, 1)].length);
}
