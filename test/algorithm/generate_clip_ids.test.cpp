#include <mbgl/test/util.hpp>

#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

using namespace mbgl;

struct Renderable {
    ClipID clip;
    bool used = true;
    bool needsClipping = true;

    bool operator==(const Renderable& rhs) const {
        return clip == rhs.clip;
    }
};

::std::ostream& operator<<(::std::ostream& os, const Renderable& rhs) {
    return os << "ClipID(" << rhs.clip << ")";
}

namespace {

// void print(const std::map<UnwrappedTileID, Renderable>& renderables) {
//     std::cout << "    EXPECT_EQ(decltype(renderables)({" << std::endl;
//     for (auto& pair : renderables) {
//         std::cout << "              { UnwrappedTileID{ " << int(pair.first.canonical.z) << ", "
//                   << (int64_t(pair.first.canonical.x) +
//                       pair.first.wrap * (1ll << pair.first.canonical.z))
//                   << ", " << pair.first.canonical.y << " }, Renderable{ ClipID{ \""
//                   << pair.second.clip.mask << "\", \"" << pair.second.clip.reference << "\" } } },"
//                   << std::endl;
//     }
//     std::cout << "          })," << std::endl;
//     std::cout << "          renderables);" << std::endl;
// }

// void print(const std::map<UnwrappedTileID, ClipID>& stencils) {
//     std::cout << "    EXPECT_EQ(decltype(stencils)({" << std::endl;
//     for (auto& pair : stencils) {
//         std::cout << "              { UnwrappedTileID{ " << int(pair.first.canonical.z) << ", "
//                   << (int64_t(pair.first.canonical.x) +
//                       pair.first.wrap * (1ll << pair.first.canonical.z))
//                   << ", " << pair.first.canonical.y << " }, ClipID{ \"" << pair.second.mask
//                   << "\", \"" << pair.second.reference << "\" } }," << std::endl;
//     }
//     std::cout << "          })," << std::endl;
//     std::cout << "          stencils);" << std::endl;
// }

} // end namespace

TEST(GenerateClipIDs, ParentAndFourChildren) {
    std::map<UnwrappedTileID, Renderable> renderables{
        { UnwrappedTileID{ 0, 0, 0 }, Renderable{ {} } },
        // All four covering children
        { UnwrappedTileID{ 1, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 0, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 1, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 1, 1 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables);

    EXPECT_EQ(decltype(renderables)({
                  { UnwrappedTileID{ 0, 0, 0 }, Renderable{ ClipID{ "00000111", "00000001" } } },
                  { UnwrappedTileID{ 1, 0, 0 }, Renderable{ ClipID{ "00000111", "00000010" } } },
                  { UnwrappedTileID{ 1, 0, 1 }, Renderable{ ClipID{ "00000111", "00000011" } } },
                  { UnwrappedTileID{ 1, 1, 0 }, Renderable{ ClipID{ "00000111", "00000100" } } },
                  { UnwrappedTileID{ 1, 1, 1 }, Renderable{ ClipID{ "00000111", "00000101" } } },
              }),
              renderables);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  // 0/0/0 is missing because it is covered by children.
                  { UnwrappedTileID{ 1, 0, 0 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 1, 0, 1 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 1, 1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 1, 1, 1 }, ClipID{ "00000111", "00000101" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, ParentAndFourChildrenNegative) {
    std::map<UnwrappedTileID, Renderable> renderables{
        { UnwrappedTileID{ 1, -2, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, -2, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, -1, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, -1, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 0, -1, 0 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables);

    EXPECT_EQ(decltype(renderables)({
                  { UnwrappedTileID{ 0, -1, 0 }, Renderable{ ClipID{ "00000111", "00000001" } } },
                  { UnwrappedTileID{ 1, -2, 0 }, Renderable{ ClipID{ "00000111", "00000010" } } },
                  { UnwrappedTileID{ 1, -2, 1 }, Renderable{ ClipID{ "00000111", "00000011" } } },
                  { UnwrappedTileID{ 1, -1, 0 }, Renderable{ ClipID{ "00000111", "00000100" } } },
                  { UnwrappedTileID{ 1, -1, 1 }, Renderable{ ClipID{ "00000111", "00000101" } } },
              }),
              renderables);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 1, -2, 0 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 1, -2, 1 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 1, -1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 1, -1, 1 }, ClipID{ "00000111", "00000101" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, NegativeParentAndMissingLevel) {
    std::map<UnwrappedTileID, Renderable> renderables{
        { UnwrappedTileID{ 1, -1, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, -1, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, -2, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, -1, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, -2, 0 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables);

    EXPECT_EQ(decltype(renderables)({
                  { UnwrappedTileID{ 1, -1, 0 }, Renderable{ ClipID{ "00000111", "00000001" } } },
                  { UnwrappedTileID{ 2, -2, 0 }, Renderable{ ClipID{ "00000111", "00000010" } } },
                  { UnwrappedTileID{ 2, -2, 1 }, Renderable{ ClipID{ "00000111", "00000011" } } },
                  { UnwrappedTileID{ 2, -1, 0 }, Renderable{ ClipID{ "00000111", "00000100" } } },
                  { UnwrappedTileID{ 2, -1, 1 }, Renderable{ ClipID{ "00000111", "00000101" } } },
              }),
              renderables);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 2, -2, 0 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 2, -2, 1 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 2, -1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 2, -1, 1 }, ClipID{ "00000111", "00000101" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, SevenOnSameLevel) {
    std::map<UnwrappedTileID, Renderable> renderables{
        // first column
        { UnwrappedTileID{ 2, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 0, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 0, 2 }, Renderable{ {} } },
        // second column
        { UnwrappedTileID{ 2, 1, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 1, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 1, 2 }, Renderable{ {} } },
        // third column
        { UnwrappedTileID{ 2, 2, 0 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables);
    EXPECT_EQ(decltype(renderables)({
                  { UnwrappedTileID{ 2, 0, 0 }, Renderable{ ClipID{ "00000111", "00000001" } } },
                  { UnwrappedTileID{ 2, 0, 1 }, Renderable{ ClipID{ "00000111", "00000010" } } },
                  { UnwrappedTileID{ 2, 0, 2 }, Renderable{ ClipID{ "00000111", "00000011" } } },
                  { UnwrappedTileID{ 2, 1, 0 }, Renderable{ ClipID{ "00000111", "00000100" } } },
                  { UnwrappedTileID{ 2, 1, 1 }, Renderable{ ClipID{ "00000111", "00000101" } } },
                  { UnwrappedTileID{ 2, 1, 2 }, Renderable{ ClipID{ "00000111", "00000110" } } },
                  { UnwrappedTileID{ 2, 2, 0 }, Renderable{ ClipID{ "00000111", "00000111" } } },
              }),
              renderables);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00000111", "00000001" } },
                  { UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 2, 0, 2 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 2, 1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 2, 1, 1 }, ClipID{ "00000111", "00000101" } },
                  { UnwrappedTileID{ 2, 1, 2 }, ClipID{ "00000111", "00000110" } },
                  { UnwrappedTileID{ 2, 2, 0 }, ClipID{ "00000111", "00000111" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, MultipleLevels) {
    std::map<UnwrappedTileID, Renderable> renderables{
        { UnwrappedTileID{ 2, 0, 0 }, Renderable{ {} } },
        // begin subtiles of (2/0/0)
        { UnwrappedTileID{ 3, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 3, 0, 1 }, Renderable{ {} } },
        // begin subtiles of (3/0/1)
        { UnwrappedTileID{ 4, 0, 2 }, Renderable{ {} } },
        { UnwrappedTileID{ 4, 1, 2 }, Renderable{ {} } },
        { UnwrappedTileID{ 4, 0, 3 }, Renderable{ {} } },
        { UnwrappedTileID{ 4, 1, 3 }, Renderable{ {} } },
        // end subtiles of (3/0/1)
        { UnwrappedTileID{ 3, 1, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 3, 1, 1 }, Renderable{ {} } },
        // end subtiles of (2/0/0)
        { UnwrappedTileID{ 2, 1, 0 }, Renderable{ {} } },
        // begin subtiles of (2/1/0)
        { UnwrappedTileID{ 3, 2, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 3, 2, 1 }, Renderable{ {} } },
        // end subtiles of (2/1/0)
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables);
    ASSERT_EQ(decltype(renderables)({
                  { UnwrappedTileID{ 2, 0, 0 }, Renderable{ ClipID{ "00001111", "00000001" } } },
                  { UnwrappedTileID{ 2, 1, 0 }, Renderable{ ClipID{ "00001111", "00000010" } } },
                  { UnwrappedTileID{ 3, 0, 0 }, Renderable{ ClipID{ "00001111", "00000011" } } },
                  { UnwrappedTileID{ 3, 0, 1 }, Renderable{ ClipID{ "00001111", "00000100" } } },
                  { UnwrappedTileID{ 3, 1, 0 }, Renderable{ ClipID{ "00001111", "00000101" } } },
                  { UnwrappedTileID{ 3, 1, 1 }, Renderable{ ClipID{ "00001111", "00000110" } } },
                  { UnwrappedTileID{ 3, 2, 0 }, Renderable{ ClipID{ "00001111", "00000111" } } },
                  { UnwrappedTileID{ 3, 2, 1 }, Renderable{ ClipID{ "00001111", "00001000" } } },
                  { UnwrappedTileID{ 4, 0, 2 }, Renderable{ ClipID{ "00001111", "00001001" } } },
                  { UnwrappedTileID{ 4, 0, 3 }, Renderable{ ClipID{ "00001111", "00001010" } } },
                  { UnwrappedTileID{ 4, 1, 2 }, Renderable{ ClipID{ "00001111", "00001011" } } },
                  { UnwrappedTileID{ 4, 1, 3 }, Renderable{ ClipID{ "00001111", "00001100" } } },
              }),
              renderables);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 2, 1, 0 }, ClipID{ "00001111", "00000010" } },
                  { UnwrappedTileID{ 3, 0, 0 }, ClipID{ "00001111", "00000011" } },
                  { UnwrappedTileID{ 3, 1, 0 }, ClipID{ "00001111", "00000101" } },
                  { UnwrappedTileID{ 3, 1, 1 }, ClipID{ "00001111", "00000110" } },
                  { UnwrappedTileID{ 3, 2, 0 }, ClipID{ "00001111", "00000111" } },
                  { UnwrappedTileID{ 3, 2, 1 }, ClipID{ "00001111", "00001000" } },
                  { UnwrappedTileID{ 4, 0, 2 }, ClipID{ "00001111", "00001001" } },
                  { UnwrappedTileID{ 4, 0, 3 }, ClipID{ "00001111", "00001010" } },
                  { UnwrappedTileID{ 4, 1, 2 }, ClipID{ "00001111", "00001011" } },
                  { UnwrappedTileID{ 4, 1, 3 }, ClipID{ "00001111", "00001100" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, Bug206) {
    std::map<UnwrappedTileID, Renderable> renderables{
        { UnwrappedTileID{ 10, 162, 395 }, Renderable{ {} } },
        { UnwrappedTileID{ 10, 162, 396 }, Renderable{ {} } },
        { UnwrappedTileID{ 10, 163, 395 }, Renderable{ {} } },
        // begin subtiles of (10/163/395)
        { UnwrappedTileID{ 11, 326, 791 }, Renderable{ {} } },
        { UnwrappedTileID{ 12, 654, 1582 }, Renderable{ {} } },
        { UnwrappedTileID{ 12, 654, 1583 }, Renderable{ {} } },
        { UnwrappedTileID{ 12, 655, 1582 }, Renderable{ {} } },
        { UnwrappedTileID{ 12, 655, 1583 }, Renderable{ {} } },
        // end subtiles of (10/163/395)
        { UnwrappedTileID{ 10, 163, 396 }, Renderable{ {} } },
        { UnwrappedTileID{ 10, 164, 395 }, Renderable{ {} } },
        { UnwrappedTileID{ 10, 164, 396 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables);
    EXPECT_EQ(
        decltype(renderables)({
            { UnwrappedTileID{ 10, 162, 395 }, Renderable{ ClipID{ "00001111", "00000001" } } },
            { UnwrappedTileID{ 10, 162, 396 }, Renderable{ ClipID{ "00001111", "00000010" } } },
            { UnwrappedTileID{ 10, 163, 395 }, Renderable{ ClipID{ "00001111", "00000011" } } },
            { UnwrappedTileID{ 10, 163, 396 }, Renderable{ ClipID{ "00001111", "00000100" } } },
            { UnwrappedTileID{ 10, 164, 395 }, Renderable{ ClipID{ "00001111", "00000101" } } },
            { UnwrappedTileID{ 10, 164, 396 }, Renderable{ ClipID{ "00001111", "00000110" } } },
            { UnwrappedTileID{ 11, 326, 791 }, Renderable{ ClipID{ "00001111", "00000111" } } },
            { UnwrappedTileID{ 12, 654, 1582 }, Renderable{ ClipID{ "00001111", "00001000" } } },
            { UnwrappedTileID{ 12, 654, 1583 }, Renderable{ ClipID{ "00001111", "00001001" } } },
            { UnwrappedTileID{ 12, 655, 1582 }, Renderable{ ClipID{ "00001111", "00001010" } } },
            { UnwrappedTileID{ 12, 655, 1583 }, Renderable{ ClipID{ "00001111", "00001011" } } },
        }),
        renderables);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 10, 162, 395 }, ClipID{ "00001111", "00000001" } },
                  { UnwrappedTileID{ 10, 162, 396 }, ClipID{ "00001111", "00000010" } },
                  { UnwrappedTileID{ 10, 163, 395 }, ClipID{ "00001111", "00000011" } },
                  { UnwrappedTileID{ 10, 163, 396 }, ClipID{ "00001111", "00000100" } },
                  { UnwrappedTileID{ 10, 164, 395 }, ClipID{ "00001111", "00000101" } },
                  { UnwrappedTileID{ 10, 164, 396 }, ClipID{ "00001111", "00000110" } },
                  { UnwrappedTileID{ 11, 326, 791 }, ClipID{ "00001111", "00000111" } },
                  { UnwrappedTileID{ 12, 654, 1582 }, ClipID{ "00001111", "00001000" } },
                  { UnwrappedTileID{ 12, 654, 1583 }, ClipID{ "00001111", "00001001" } },
                  { UnwrappedTileID{ 12, 655, 1582 }, ClipID{ "00001111", "00001010" } },
                  { UnwrappedTileID{ 12, 655, 1583 }, ClipID{ "00001111", "00001011" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, MultipleSources) {
    std::map<UnwrappedTileID, Renderable> renderables1{
        { UnwrappedTileID{ 0, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 1, 1 }, Renderable{ {} } },
        // Differing children
        { UnwrappedTileID{ 2, 2, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 2, 2 }, Renderable{ {} } },
    };
    std::map<UnwrappedTileID, Renderable> renderables2{
        { UnwrappedTileID{ 0, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 1, 1 }, Renderable{ {} } },
        // Differing children
        { UnwrappedTileID{ 2, 1, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 2, 2 }, Renderable{ {} } },
    };
    std::map<UnwrappedTileID, Renderable> renderables3{
        { UnwrappedTileID{ 1, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 0, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 1, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 1, 1, 1 }, Renderable{ {} } },
        // Differing children
        { UnwrappedTileID{ 2, 1, 1 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables1);
    generator.update(renderables2);
    generator.update(renderables3);
    EXPECT_EQ(decltype(renderables1)({
                  { UnwrappedTileID{ 0, 0, 0 }, Renderable{ ClipID{ "00000111", "00000001" } } },
                  { UnwrappedTileID{ 1, 1, 1 }, Renderable{ ClipID{ "00000111", "00000010" } } },
                  { UnwrappedTileID{ 2, 2, 1 }, Renderable{ ClipID{ "00000111", "00000011" } } },
                  { UnwrappedTileID{ 2, 2, 2 }, Renderable{ ClipID{ "00000111", "00000100" } } },
              }),
              renderables1);
    EXPECT_EQ(decltype(renderables2)({
                  { UnwrappedTileID{ 0, 0, 0 }, Renderable{ ClipID{ "00011000", "00001000" } } },
                  { UnwrappedTileID{ 1, 1, 1 }, Renderable{ ClipID{ "00011111", "00000010" } } },
                  { UnwrappedTileID{ 2, 1, 1 }, Renderable{ ClipID{ "00011000", "00010000" } } },
                  { UnwrappedTileID{ 2, 2, 2 }, Renderable{ ClipID{ "00011111", "00000100" } } },
              }),
              renderables2);
    EXPECT_EQ(decltype(renderables3)({
                  { UnwrappedTileID{ 1, 0, 0 }, Renderable{ ClipID{ "11100000", "00100000" } } },
                  { UnwrappedTileID{ 1, 0, 1 }, Renderable{ ClipID{ "11100000", "01000000" } } },
                  { UnwrappedTileID{ 1, 1, 0 }, Renderable{ ClipID{ "11100000", "01100000" } } },
                  { UnwrappedTileID{ 1, 1, 1 }, Renderable{ ClipID{ "11100000", "10000000" } } },
                  { UnwrappedTileID{ 2, 1, 1 }, Renderable{ ClipID{ "11111000", "00010000" } } },
              }),
              renderables3);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 1, 0, 0 }, ClipID{ "11111111", "00101001" } },
                  { UnwrappedTileID{ 1, 0, 1 }, ClipID{ "11111111", "01001001" } },
                  { UnwrappedTileID{ 1, 1, 0 }, ClipID{ "11111111", "01101001" } },
                  { UnwrappedTileID{ 1, 1, 1 }, ClipID{ "11111111", "10000010" } },
                  { UnwrappedTileID{ 2, 1, 1 }, ClipID{ "11111111", "00010001" } },
                  { UnwrappedTileID{ 2, 2, 1 }, ClipID{ "11111111", "01101011" } },
                  { UnwrappedTileID{ 2, 2, 2 }, ClipID{ "11111111", "10000100" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, DuplicateIDs) {
    std::map<UnwrappedTileID, Renderable> renderables1{
        { UnwrappedTileID{ 2, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 0, 1 }, Renderable{ {} } },
    };
    std::map<UnwrappedTileID, Renderable> renderables2{
        { UnwrappedTileID{ 2, 0, 0 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 0, 1 }, Renderable{ {} } },
        { UnwrappedTileID{ 2, 0, 1 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables1);
    generator.update(renderables2);
    EXPECT_EQ(decltype(renderables1)({
                  { UnwrappedTileID{ 2, 0, 0 }, Renderable{ ClipID{ "00000011", "00000001" } } },
                  { UnwrappedTileID{ 2, 0, 1 }, Renderable{ ClipID{ "00000011", "00000010" } } },
              }),
              renderables1);
    EXPECT_EQ(decltype(renderables2)({
                  { UnwrappedTileID{ 2, 0, 0 }, Renderable{ ClipID{ "00000011", "00000001" } } },
                  { UnwrappedTileID{ 2, 0, 1 }, Renderable{ ClipID{ "00000011", "00000010" } } },
              }),
              renderables2);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00000011", "00000001" } },
                  { UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000011", "00000010" } },
              }),
              stencils);
}

TEST(GenerateClipIDs, SecondSourceHasParentOfFirstSource) {
    std::map<UnwrappedTileID, Renderable> renderables1{
        { UnwrappedTileID{ 1, 0, 0 }, Renderable{ {} } },
    };
    std::map<UnwrappedTileID, Renderable> renderables2{
        { UnwrappedTileID{ 0, 0, 0 }, Renderable{ {} } },
        // Same as in renderables1, but has a parent that it knocks out.
        { UnwrappedTileID{ 1, 0, 0 }, Renderable{ {} } },
    };
    std::map<UnwrappedTileID, Renderable> renderables3{
        { UnwrappedTileID{ 0, 0, 0 }, Renderable{ {} } },
    };

    algorithm::ClipIDGenerator generator;
    generator.update(renderables1);
    generator.update(renderables2);
    generator.update(renderables3);
    EXPECT_EQ(decltype(renderables1)({
                  { UnwrappedTileID{ 1, 0, 0 }, Renderable{ ClipID{ "00000001", "00000001" } } },
              }),
              renderables1);
    EXPECT_EQ(decltype(renderables2)({
                  { UnwrappedTileID{ 0, 0, 0 }, Renderable{ ClipID{ "00000010", "00000010" } } },
                  { UnwrappedTileID{ 1, 0, 0 }, Renderable{ ClipID{ "00000011", "00000001" } } },
              }),
              renderables2);
    EXPECT_EQ(decltype(renderables3)({
                  { UnwrappedTileID{ 0, 0, 0 }, Renderable{ ClipID{ "00000100", "00000100" } } },
              }),
              renderables3);

    const auto stencils = generator.getStencils();
    EXPECT_EQ(decltype(stencils)({
                  { UnwrappedTileID{ 0, 0, 0 }, ClipID{ "00000110", "00000110" } },
                  { UnwrappedTileID{ 1, 0, 0 }, ClipID{ "00000111", "00000101" } },
              }),
              stencils);
}
