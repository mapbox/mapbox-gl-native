#include <iostream>
#include "../fixtures/util.hpp"

#include <algorithm>

#include <mbgl/util/clip_id.hpp>
#include <mbgl/map/tile.hpp>

using namespace mbgl;

using Stencil = std::pair<const TileID, ClipID>;

template <typename T> void generate(ClipIDGenerator& generator, const T &sources) {
    for (size_t j = 0; j < sources.size(); j++) {
        std::forward_list<Tile *> tile_ptrs;
        std::transform(sources[j].begin(), sources[j].end(), std::front_inserter(tile_ptrs), [](const std::shared_ptr<Tile> &tile) { return tile.get(); });
        generator.update(tile_ptrs);
    }
}

void print(const std::vector<std::vector<std::shared_ptr<Tile>>> &sources) {
    for (size_t j = 0; j < sources.size(); j++) {
        for (size_t i = 0; i < sources[j].size(); i++) {
            std::cout << "    ASSERT_EQ(ClipID(\"" << sources[j][i]->clip.mask << "\", \"" << sources[j][i]->clip.reference << "\"), sources[" << j << "][" << i << "]->clip);\n";
        }
    }
}

void print(const std::map<TileID, ClipID>& stencils) {
    std::cout << "    auto it = stencils.begin();\n";
    std::cout << "    ASSERT_EQ(" << stencils.size() << ", stencils.size());\n";
    for (auto& stencil : stencils) {
        std::cout << "    ASSERT_EQ(Stencil(TileID{ " << (int)stencil.first.z << ", "
                  << stencil.first.x << ", " << stencil.first.y << ", "
                  << (int)stencil.first.sourceZ << " }, { \"" << stencil.second.mask.to_string()
                  << "\", \"" << stencil.second.reference.to_string() << "\"}), *it++);\n";
    }
    std::cout << "    ASSERT_EQ(stencils.end(), it);\n";
}

TEST(ClipIDs, ParentAndFourChildren) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 1, 0, 0, 1 }),
            std::make_shared<Tile>(TileID { 1, 0, 1, 1 }),
            std::make_shared<Tile>(TileID { 1, 1, 0, 1 }),
            std::make_shared<Tile>(TileID { 1, 1, 1, 1 }),
            std::make_shared<Tile>(TileID { 0, 0, 0, 0 }),
        },
    };

    ClipIDGenerator generator;
    generate(generator, sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][4]->clip);

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(4, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 1, 0, 0, 1 }, { "00000111", "00000010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, 0, 1, 1 }, { "00000111", "00000011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, 1, 0, 1 }, { "00000111", "00000100"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, 1, 1, 1 }, { "00000111", "00000101"}), *it++);
    ASSERT_EQ(stencils.end(), it);

}

TEST(ClipIDs, ParentAndFourChildrenNegative) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 1, -2, 0, 1 }),
            std::make_shared<Tile>(TileID { 1, -2, 1, 1 }),
            std::make_shared<Tile>(TileID { 1, -1, 0, 1 }),
            std::make_shared<Tile>(TileID { 1, -1, 1, 1 }),
            std::make_shared<Tile>(TileID { 0, -1, 0, 0 }),
        },
    };

    ClipIDGenerator generator;
    generate(generator, sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][4]->clip);

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(4, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 1, -2, 0, 1 }, { "00000111", "00000010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, -2, 1, 1 }, { "00000111", "00000011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, -1, 0, 1 }, { "00000111", "00000100"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, -1, 1, 1 }, { "00000111", "00000101"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}

TEST(ClipIDs, NegativeParentAndMissingLevel) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 1, -1, 0, 1 }),
            std::make_shared<Tile>(TileID { 2, -1, 0, 2 }),
            std::make_shared<Tile>(TileID { 2, -2, 1, 2 }),
            std::make_shared<Tile>(TileID { 2, -1, 1, 2 }),
            std::make_shared<Tile>(TileID { 2, -2, 0, 2 }),
        },
    };

    ClipIDGenerator generator;
    generate(generator, sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][4]->clip);

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(4, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 2, -2, 0, 2 }, { "00000111", "00000010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, -2, 1, 2 }, { "00000111", "00000011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, -1, 0, 2 }, { "00000111", "00000100"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, -1, 1, 2 }, { "00000111", "00000101"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}


TEST(ClipIDs, SevenOnSameLevel) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 2, 0, 0, 2 }),
            std::make_shared<Tile>(TileID { 2, 0, 1, 2 }),
            std::make_shared<Tile>(TileID { 2, 0, 2, 2 }),
            std::make_shared<Tile>(TileID { 2, 1, 0, 2 }),
            std::make_shared<Tile>(TileID { 2, 1, 1, 2 }),
            std::make_shared<Tile>(TileID { 2, 1, 2, 2 }),
            std::make_shared<Tile>(TileID { 2, 2, 0, 2 }),
        },
    };

    ClipIDGenerator generator;
    generate(generator, sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][3]->clip);
    ASSERT_EQ(ClipID("00000111", "00000101"), sources[0][4]->clip);
    ASSERT_EQ(ClipID("00000111", "00000110"), sources[0][5]->clip);
    ASSERT_EQ(ClipID("00000111", "00000111"), sources[0][6]->clip);

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(7, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 2, 0, 0, 2 }, { "00000111", "00000001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 0, 1, 2 }, { "00000111", "00000010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 0, 2, 2 }, { "00000111", "00000011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 1, 0, 2 }, { "00000111", "00000100"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 1, 1, 2 }, { "00000111", "00000101"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 1, 2, 2 }, { "00000111", "00000110"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 2, 0, 2 }, { "00000111", "00000111"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}

TEST(ClipIDs, MultipleLevels) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 2, 0, 0, 2 }),
                std::make_shared<Tile>(TileID { 3, 0, 0, 3 }),
                std::make_shared<Tile>(TileID { 3, 0, 1, 3 }),
                    std::make_shared<Tile>(TileID { 4, 0, 2, 4 }),
                    std::make_shared<Tile>(TileID { 4, 1, 2, 4 }),
                    std::make_shared<Tile>(TileID { 4, 0, 3, 4 }),
                    std::make_shared<Tile>(TileID { 4, 1, 3, 4 }),
                std::make_shared<Tile>(TileID { 3, 1, 0, 3 }),
                std::make_shared<Tile>(TileID { 3, 1, 1, 3 }),
            std::make_shared<Tile>(TileID { 2, 1, 0, 2 }),
                std::make_shared<Tile>(TileID { 3, 2, 0, 3 }),
                std::make_shared<Tile>(TileID { 3, 2, 1, 3 }),
        },
    };

    ClipIDGenerator generator;
    generate(generator, sources);
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

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(10, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 2, 1, 0, 2 }, { "00001111", "00000010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 3, 0, 0, 3 }, { "00001111", "00000011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 3, 1, 0, 3 }, { "00001111", "00000101"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 3, 1, 1, 3 }, { "00001111", "00000110"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 3, 2, 0, 3 }, { "00001111", "00000111"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 3, 2, 1, 3 }, { "00001111", "00001000"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 4, 0, 2, 4 }, { "00001111", "00001001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 4, 0, 3, 4 }, { "00001111", "00001010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 4, 1, 2, 4 }, { "00001111", "00001011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 4, 1, 3, 4 }, { "00001111", "00001100"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}


TEST(ClipIDs, Bug206) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 10, 162, 395, 10 }),
            std::make_shared<Tile>(TileID { 10, 162, 396, 10 }),
            std::make_shared<Tile>(TileID { 10, 163, 395, 10 }),
                std::make_shared<Tile>(TileID { 11, 326, 791, 11 }),
                std::make_shared<Tile>(TileID { 12, 654, 1582, 12 }),
                std::make_shared<Tile>(TileID { 12, 654, 1583, 12 }),
                std::make_shared<Tile>(TileID { 12, 655, 1582, 12 }),
                std::make_shared<Tile>(TileID { 12, 655, 1583, 12 }),
            std::make_shared<Tile>(TileID { 10, 163, 396, 10 }),
            std::make_shared<Tile>(TileID { 10, 164, 395, 10 }),
            std::make_shared<Tile>(TileID { 10, 164, 396, 10 }),
        },
    };

    ClipIDGenerator generator;
    generate(generator, sources);
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

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(11, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 10, 162, 395, 10 }, { "00001111", "00000001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 10, 162, 396, 10 }, { "00001111", "00000010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 10, 163, 395, 10 }, { "00001111", "00000011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 10, 163, 396, 10 }, { "00001111", "00000100"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 10, 164, 395, 10 }, { "00001111", "00000101"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 10, 164, 396, 10 }, { "00001111", "00000110"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 11, 326, 791, 11 }, { "00001111", "00000111"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 12, 654, 1582, 12 }, { "00001111", "00001000"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 12, 654, 1583, 12 }, { "00001111", "00001001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 12, 655, 1582, 12 }, { "00001111", "00001010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 12, 655, 1583, 12 }, { "00001111", "00001011"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}


TEST(ClipIDs, MultipleSources) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 0, 0, 0, 0 }),
            std::make_shared<Tile>(TileID { 1, 1, 1, 1 }),
            std::make_shared<Tile>(TileID { 2, 2, 1, 2 }),
            std::make_shared<Tile>(TileID { 2, 2, 2, 2 }),
        },
        {
            std::make_shared<Tile>(TileID { 0, 0, 0, 0 }),
            std::make_shared<Tile>(TileID { 1, 1, 1, 1 }),
            std::make_shared<Tile>(TileID { 2, 1, 1, 2 }),
            std::make_shared<Tile>(TileID { 2, 2, 2, 2 }),
        },
        {
            std::make_shared<Tile>(TileID { 1, 0, 0, 1 }),
            std::make_shared<Tile>(TileID { 1, 0, 1, 1 }),
            std::make_shared<Tile>(TileID { 1, 1, 0, 1 }),
            std::make_shared<Tile>(TileID { 1, 1, 1, 1 }),
            std::make_shared<Tile>(TileID { 2, 1, 1, 2 }),
        },
    };

    ClipIDGenerator generator;
    generate(generator, sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000111", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000111", "00000010"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000111", "00000011"), sources[0][2]->clip);
    ASSERT_EQ(ClipID("00000111", "00000100"), sources[0][3]->clip);

    ASSERT_EQ(ClipID("00011000", "00001000"), sources[1][0]->clip);
    ASSERT_EQ(ClipID("00011111", "00000010"), sources[1][1]->clip);
    ASSERT_EQ(ClipID("00011000", "00010000"), sources[1][2]->clip);
    ASSERT_EQ(ClipID("00011111", "00000100"), sources[1][3]->clip);

    ASSERT_EQ(ClipID("11100000", "00100000"), sources[2][0]->clip);
    ASSERT_EQ(ClipID("11100000", "01000000"), sources[2][1]->clip);
    ASSERT_EQ(ClipID("11100000", "01100000"), sources[2][2]->clip);
    ASSERT_EQ(ClipID("11100000", "10000000"), sources[2][3]->clip);
    ASSERT_EQ(ClipID("11111000", "00010000"), sources[2][4]->clip);

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(7, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 1, 0, 0, 1 }, { "11111111", "00101001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, 0, 1, 1 }, { "11111111", "01001001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, 1, 0, 1 }, { "11111111", "01101001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, 1, 1, 1 }, { "11111111", "10000010"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 1, 1, 2 }, { "11111111", "00010001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 2, 1, 2 }, { "11111111", "01101011"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 2, 2, 2 }, { "11111111", "10000100"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}


TEST(ClipIDs, DuplicateIDs) {
    const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 2, 0, 0, 2 }),
            std::make_shared<Tile>(TileID { 2, 0, 1, 2 }),
        },
        {
            std::make_shared<Tile>(TileID { 2, 0, 0, 2 }),
            std::make_shared<Tile>(TileID { 2, 0, 1, 2 }),
            std::make_shared<Tile>(TileID { 2, 0, 1, 2 }),
        }
    };

    ClipIDGenerator generator;
    generate(generator, sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000011", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000011", "00000010"), sources[0][1]->clip);
    ASSERT_EQ(ClipID("00000011", "00000001"), sources[1][0]->clip);
    ASSERT_EQ(ClipID("00000011", "00000010"), sources[1][1]->clip);
    ASSERT_EQ(ClipID("00000011", "00000010"), sources[1][2]->clip);

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(2, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 2, 0, 0, 2 }, { "00000011", "00000001"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 2, 0, 1, 2 }, { "00000011", "00000010"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}

TEST(ClipIDs, SecondSourceHasParentOfFirstSource) {
        const std::vector<std::vector<std::shared_ptr<Tile>>> sources = {
        {
            std::make_shared<Tile>(TileID { 1, 0, 0, 1 }),
        },
        {
            std::make_shared<Tile>(TileID { 0, 0, 0, 0 }),
            std::make_shared<Tile>(TileID { 1, 0, 0, 1 }),
        },
        {
            std::make_shared<Tile>(TileID { 0, 0, 0, 0 }),
        }
    };

    ClipIDGenerator generator;
    generate(generator, sources);
    // print(sources);

    ASSERT_EQ(ClipID("00000001", "00000001"), sources[0][0]->clip);
    ASSERT_EQ(ClipID("00000010", "00000010"), sources[1][0]->clip);
    ASSERT_EQ(ClipID("00000011", "00000001"), sources[1][1]->clip);

    const auto stencils = generator.getStencils();
    // print(stencils);

    auto it = stencils.begin();
    ASSERT_EQ(2, stencils.size());
    ASSERT_EQ(Stencil(TileID{ 0, 0, 0, 0 }, { "00000110", "00000110"}), *it++);
    ASSERT_EQ(Stencil(TileID{ 1, 0, 0, 1 }, { "00000111", "00000101"}), *it++);
    ASSERT_EQ(stencils.end(), it);
}
