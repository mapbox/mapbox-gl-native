#include <mbgl/test/util.hpp>

#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

using namespace mbgl;

struct Renderable {
    UnwrappedTileID id;
    ClipID clip;
    bool used;
    bool needsClipping;

    Renderable(UnwrappedTileID id_,
               ClipID clip_,
               bool used_ = true,
               bool needsClipping_ = true)
        : id(std::move(id_)),
          clip(std::move(clip_)),
          used(used_),
          needsClipping(needsClipping_) {}

    bool operator==(const Renderable& rhs) const {
        return id == rhs.id && clip == rhs.clip;
    }
};

::std::ostream& operator<<(::std::ostream& os, const Renderable& rhs) {
    return os << "Renderable{ " << rhs.id <<  ", " << rhs.clip << " }";
}

TEST(GenerateClipIDs, ParentAndFourChildren) {
    std::vector<Renderable> renderables{
        Renderable{ UnwrappedTileID{ 0, 0, 0 }, {} },
        // All four covering children
        Renderable{ UnwrappedTileID{ 1, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, 0, 1 }, {} },
        Renderable{ UnwrappedTileID{ 1, 1, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, 1, 1 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables.begin(), renderables.end() });

    EXPECT_EQ(decltype(renderables)({
                  Renderable{ UnwrappedTileID{ 0, 0, 0 }, ClipID{ "00000111", "00000001" } },
                  Renderable{ UnwrappedTileID{ 1, 0, 0 }, ClipID{ "00000111", "00000010" } },
                  Renderable{ UnwrappedTileID{ 1, 0, 1 }, ClipID{ "00000111", "00000011" } },
                  Renderable{ UnwrappedTileID{ 1, 1, 0 }, ClipID{ "00000111", "00000100" } },
                  Renderable{ UnwrappedTileID{ 1, 1, 1 }, ClipID{ "00000111", "00000101" } },
              }),
              renderables);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
                  // 0/0/0 is missing because it is covered by children.
                  { UnwrappedTileID{ 1, 0, 0 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 1, 0, 1 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 1, 1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 1, 1, 1 }, ClipID{ "00000111", "00000101" } },
              }),
              clipIDs);
}

TEST(GenerateClipIDs, ParentAndFourChildrenNegative) {
    std::vector<Renderable> renderables{
        Renderable{ UnwrappedTileID{ 1, -2, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, -2, 1 }, {} },
        Renderable{ UnwrappedTileID{ 1, -1, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, -1, 1 }, {} },
        Renderable{ UnwrappedTileID{ 0, -1, 0 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables.begin(), renderables.end() });

    EXPECT_EQ(decltype(renderables)({
                  Renderable{ UnwrappedTileID{ 1, -2, 0 }, ClipID{ "00000111", "00000010" } },
                  Renderable{ UnwrappedTileID{ 1, -2, 1 }, ClipID{ "00000111", "00000011" } },
                  Renderable{ UnwrappedTileID{ 1, -1, 0 }, ClipID{ "00000111", "00000100" } },
                  Renderable{ UnwrappedTileID{ 1, -1, 1 }, ClipID{ "00000111", "00000101" } },
                  Renderable{ UnwrappedTileID{ 0, -1, 0 }, ClipID{ "00000111", "00000001" } },
              }),
              renderables);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
                  { UnwrappedTileID{ 1, -2, 0 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 1, -2, 1 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 1, -1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 1, -1, 1 }, ClipID{ "00000111", "00000101" } },
              }),
              clipIDs);
}

TEST(GenerateClipIDs, NegativeParentAndMissingLevel) {
    std::vector<Renderable> renderables{
        Renderable{ UnwrappedTileID{ 1, -1, 0 }, {} },
        Renderable{ UnwrappedTileID{ 2, -1, 0 }, {} },
        Renderable{ UnwrappedTileID{ 2, -2, 1 }, {} },
        Renderable{ UnwrappedTileID{ 2, -1, 1 }, {} },
        Renderable{ UnwrappedTileID{ 2, -2, 0 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables.begin(), renderables.end() });

    EXPECT_EQ(decltype(renderables)({
                  Renderable{ UnwrappedTileID{ 1, -1, 0 }, ClipID{ "00000111", "00000001" } },
                  Renderable{ UnwrappedTileID{ 2, -1, 0 }, ClipID{ "00000111", "00000100" } },
                  Renderable{ UnwrappedTileID{ 2, -2, 1 }, ClipID{ "00000111", "00000011" } },
                  Renderable{ UnwrappedTileID{ 2, -1, 1 }, ClipID{ "00000111", "00000101" } },
                  Renderable{ UnwrappedTileID{ 2, -2, 0 }, ClipID{ "00000111", "00000010" } },
              }),
              renderables);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
                  { UnwrappedTileID{ 2, -2, 0 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 2, -2, 1 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 2, -1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 2, -1, 1 }, ClipID{ "00000111", "00000101" } },
              }),
              clipIDs);
}

TEST(GenerateClipIDs, SevenOnSameLevel) {
    std::vector<Renderable> renderables{
        // first column
        Renderable{ UnwrappedTileID{ 2, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 2, 0, 1 }, {} },
        Renderable{ UnwrappedTileID{ 2, 0, 2 }, {} },
        // second column
        Renderable{ UnwrappedTileID{ 2, 1, 0 }, {} },
        Renderable{ UnwrappedTileID{ 2, 1, 1 }, {} },
        Renderable{ UnwrappedTileID{ 2, 1, 2 }, {} },
        // third column
        Renderable{ UnwrappedTileID{ 2, 2, 0 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables.begin(), renderables.end() });
    EXPECT_EQ(decltype(renderables)({
                  Renderable{ UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00000111", "00000001" } },
                  Renderable{ UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000111", "00000010" } },
                  Renderable{ UnwrappedTileID{ 2, 0, 2 }, ClipID{ "00000111", "00000011" } },
                  Renderable{ UnwrappedTileID{ 2, 1, 0 }, ClipID{ "00000111", "00000100" } },
                  Renderable{ UnwrappedTileID{ 2, 1, 1 }, ClipID{ "00000111", "00000101" } },
                  Renderable{ UnwrappedTileID{ 2, 1, 2 }, ClipID{ "00000111", "00000110" } },
                  Renderable{ UnwrappedTileID{ 2, 2, 0 }, ClipID{ "00000111", "00000111" } },
              }),
              renderables);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
                  { UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00000111", "00000001" } },
                  { UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000111", "00000010" } },
                  { UnwrappedTileID{ 2, 0, 2 }, ClipID{ "00000111", "00000011" } },
                  { UnwrappedTileID{ 2, 1, 0 }, ClipID{ "00000111", "00000100" } },
                  { UnwrappedTileID{ 2, 1, 1 }, ClipID{ "00000111", "00000101" } },
                  { UnwrappedTileID{ 2, 1, 2 }, ClipID{ "00000111", "00000110" } },
                  { UnwrappedTileID{ 2, 2, 0 }, ClipID{ "00000111", "00000111" } },
              }),
              clipIDs);
}

TEST(GenerateClipIDs, MultipleLevels) {
    std::vector<Renderable> renderables{
        Renderable{ UnwrappedTileID{ 2, 0, 0 }, {} },
        // begin subtiles of (2/0/0)
        Renderable{ UnwrappedTileID{ 3, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 3, 0, 1 }, {} },
        // begin subtiles of (3/0/1)
        Renderable{ UnwrappedTileID{ 4, 0, 2 }, {} },
        Renderable{ UnwrappedTileID{ 4, 1, 2 }, {} },
        Renderable{ UnwrappedTileID{ 4, 0, 3 }, {} },
        Renderable{ UnwrappedTileID{ 4, 1, 3 }, {} },
        // end subtiles of (3/0/1)
        Renderable{ UnwrappedTileID{ 3, 1, 0 }, {} },
        Renderable{ UnwrappedTileID{ 3, 1, 1 }, {} },
        // end subtiles of (2/0/0)
        Renderable{ UnwrappedTileID{ 2, 1, 0 }, {} },
        // begin subtiles of (2/1/0)
        Renderable{ UnwrappedTileID{ 3, 2, 0 }, {} },
        Renderable{ UnwrappedTileID{ 3, 2, 1 }, {} },
        // end subtiles of (2/1/0)
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables.begin(), renderables.end() });
    ASSERT_EQ(decltype(renderables)({
                  Renderable{ UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00001111", "00000001" } },
                  Renderable{ UnwrappedTileID{ 3, 0, 0 }, ClipID{ "00001111", "00000011" } },
                  Renderable{ UnwrappedTileID{ 3, 0, 1 }, ClipID{ "00001111", "00000100" } },
                  Renderable{ UnwrappedTileID{ 4, 0, 2 }, ClipID{ "00001111", "00001001" } },
                  Renderable{ UnwrappedTileID{ 4, 1, 2 }, ClipID{ "00001111", "00001011" } },
                  Renderable{ UnwrappedTileID{ 4, 0, 3 }, ClipID{ "00001111", "00001010" } },
                  Renderable{ UnwrappedTileID{ 4, 1, 3 }, ClipID{ "00001111", "00001100" } },
                  Renderable{ UnwrappedTileID{ 3, 1, 0 }, ClipID{ "00001111", "00000101" } },
                  Renderable{ UnwrappedTileID{ 3, 1, 1 }, ClipID{ "00001111", "00000110" } },
                  Renderable{ UnwrappedTileID{ 2, 1, 0 }, ClipID{ "00001111", "00000010" } },
                  Renderable{ UnwrappedTileID{ 3, 2, 0 }, ClipID{ "00001111", "00000111" } },
                  Renderable{ UnwrappedTileID{ 3, 2, 1 }, ClipID{ "00001111", "00001000" } },
              }),
              renderables);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
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
              clipIDs);
}

TEST(GenerateClipIDs, Bug206) {
    std::vector<Renderable> renderables{
        Renderable{ UnwrappedTileID{ 10, 162, 395 }, {} },
        Renderable{ UnwrappedTileID{ 10, 162, 396 }, {} },
        Renderable{ UnwrappedTileID{ 10, 163, 395 }, {} },
        // begin subtiles of (10/163/395)
        Renderable{ UnwrappedTileID{ 11, 326, 791 }, {} },
        Renderable{ UnwrappedTileID{ 12, 654, 1582 }, {} },
        Renderable{ UnwrappedTileID{ 12, 654, 1583 }, {} },
        Renderable{ UnwrappedTileID{ 12, 655, 1582 }, {} },
        Renderable{ UnwrappedTileID{ 12, 655, 1583 }, {} },
        // end subtiles of (10/163/395)
        Renderable{ UnwrappedTileID{ 10, 163, 396 }, {} },
        Renderable{ UnwrappedTileID{ 10, 164, 395 }, {} },
        Renderable{ UnwrappedTileID{ 10, 164, 396 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables.begin(), renderables.end() });
    EXPECT_EQ(
        decltype(renderables)({
            Renderable{ UnwrappedTileID{ 10, 162, 395 }, ClipID{ "00001111", "00000001" } },
            Renderable{ UnwrappedTileID{ 10, 162, 396 }, ClipID{ "00001111", "00000010" } },
            Renderable{ UnwrappedTileID{ 10, 163, 395 }, ClipID{ "00001111", "00000011" } },
            Renderable{ UnwrappedTileID{ 11, 326, 791 }, ClipID{ "00001111", "00000111" } },
            Renderable{ UnwrappedTileID{ 12, 654, 1582 }, ClipID{ "00001111", "00001000" } },
            Renderable{ UnwrappedTileID{ 12, 654, 1583 }, ClipID{ "00001111", "00001001" } },
            Renderable{ UnwrappedTileID{ 12, 655, 1582 }, ClipID{ "00001111", "00001010" } },
            Renderable{ UnwrappedTileID{ 12, 655, 1583 }, ClipID{ "00001111", "00001011" } },
            Renderable{ UnwrappedTileID{ 10, 163, 396 }, ClipID{ "00001111", "00000100" } },
            Renderable{ UnwrappedTileID{ 10, 164, 395 }, ClipID{ "00001111", "00000101" } },
            Renderable{ UnwrappedTileID{ 10, 164, 396 }, ClipID{ "00001111", "00000110" } },
        }),
        renderables);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
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
              clipIDs);
}

TEST(GenerateClipIDs, MultipleSources) {
    std::vector<Renderable> renderables1{
        Renderable{ UnwrappedTileID{ 0, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, 1, 1 }, {} },
        // Differing children
        Renderable{ UnwrappedTileID{ 2, 2, 1 }, {} },
        Renderable{ UnwrappedTileID{ 2, 2, 2 }, {} },
    };
    std::vector<Renderable> renderables2{
        Renderable{ UnwrappedTileID{ 0, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, 1, 1 }, {} },
        // Differing children
        Renderable{ UnwrappedTileID{ 2, 1, 1 }, {} },
        Renderable{ UnwrappedTileID{ 2, 2, 2 }, {} },
    };
    std::vector<Renderable> renderables3{
        Renderable{ UnwrappedTileID{ 1, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, 0, 1 }, {} },
        Renderable{ UnwrappedTileID{ 1, 1, 0 }, {} },
        Renderable{ UnwrappedTileID{ 1, 1, 1 }, {} },
        // Differing children
        Renderable{ UnwrappedTileID{ 2, 1, 1 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables1.begin(), renderables1.end() });
    generator.update<Renderable>({ renderables2.begin(), renderables2.end() });
    generator.update<Renderable>({ renderables3.begin(), renderables3.end() });
    EXPECT_EQ(decltype(renderables1)({
                  Renderable{ UnwrappedTileID{ 0, 0, 0 }, ClipID{ "00000111", "00000001" } },
                  Renderable{ UnwrappedTileID{ 1, 1, 1 }, ClipID{ "00000111", "00000010" } },
                  Renderable{ UnwrappedTileID{ 2, 2, 1 }, ClipID{ "00000111", "00000011" } },
                  Renderable{ UnwrappedTileID{ 2, 2, 2 }, ClipID{ "00000111", "00000100" } },
              }),
              renderables1);
    EXPECT_EQ(decltype(renderables2)({
                  Renderable{ UnwrappedTileID{ 0, 0, 0 }, ClipID{ "00011000", "00001000" } },
                  Renderable{ UnwrappedTileID{ 1, 1, 1 }, ClipID{ "00011111", "00000010" } },
                  Renderable{ UnwrappedTileID{ 2, 1, 1 }, ClipID{ "00011000", "00010000" } },
                  Renderable{ UnwrappedTileID{ 2, 2, 2 }, ClipID{ "00011111", "00000100" } },
              }),
              renderables2);
    EXPECT_EQ(decltype(renderables3)({
                  Renderable{ UnwrappedTileID{ 1, 0, 0 }, ClipID{ "11100000", "00100000" } },
                  Renderable{ UnwrappedTileID{ 1, 0, 1 }, ClipID{ "11100000", "01000000" } },
                  Renderable{ UnwrappedTileID{ 1, 1, 0 }, ClipID{ "11100000", "01100000" } },
                  Renderable{ UnwrappedTileID{ 1, 1, 1 }, ClipID{ "11100000", "10000000" } },
                  Renderable{ UnwrappedTileID{ 2, 1, 1 }, ClipID{ "11111000", "00010000" } },
              }),
              renderables3);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
                  { UnwrappedTileID{ 1, 0, 0 }, ClipID{ "11111111", "00101001" } },
                  { UnwrappedTileID{ 1, 0, 1 }, ClipID{ "11111111", "01001001" } },
                  { UnwrappedTileID{ 1, 1, 0 }, ClipID{ "11111111", "01101001" } },
                  { UnwrappedTileID{ 1, 1, 1 }, ClipID{ "11111111", "10000010" } },
                  { UnwrappedTileID{ 2, 1, 1 }, ClipID{ "11111111", "00010001" } },
                  { UnwrappedTileID{ 2, 2, 1 }, ClipID{ "11111111", "01101011" } },
                  { UnwrappedTileID{ 2, 2, 2 }, ClipID{ "11111111", "10000100" } },
              }),
              clipIDs);
}

TEST(GenerateClipIDs, DuplicateIDs) {
    std::vector<Renderable> renderables1{
        Renderable{ UnwrappedTileID{ 2, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 2, 0, 1 }, {} },
    };
    std::vector<Renderable> renderables2{
        Renderable{ UnwrappedTileID{ 2, 0, 0 }, {} },
        Renderable{ UnwrappedTileID{ 2, 0, 1 }, {} },
        Renderable{ UnwrappedTileID{ 2, 0, 1 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables1.begin(), renderables1.end() });
    generator.update<Renderable>({ renderables2.begin(), renderables2.end() });
    EXPECT_EQ(decltype(renderables1)({
                  Renderable{ UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00000011", "00000001" } },
                  Renderable{ UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000011", "00000010" } },
              }),
              renderables1);
    EXPECT_EQ(decltype(renderables2)({
                  Renderable{ UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00000011", "00000001" } },
                  Renderable{ UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000011", "00000010" } },
                  Renderable{ UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000011", "00000010" } },
              }),
              renderables2);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
                  { UnwrappedTileID{ 2, 0, 0 }, ClipID{ "00000011", "00000001" } },
                  { UnwrappedTileID{ 2, 0, 1 }, ClipID{ "00000011", "00000010" } },
              }),
              clipIDs);
}

TEST(GenerateClipIDs, SecondSourceHasParentOfFirstSource) {
    std::vector<Renderable> renderables1{
        Renderable{ UnwrappedTileID{ 1, 0, 0 }, {} },
    };
    std::vector<Renderable> renderables2{
        Renderable{ UnwrappedTileID{ 0, 0, 0 }, {} },
        // Same as in renderables1, but has a parent that it knocks out.
        Renderable{ UnwrappedTileID{ 1, 0, 0 }, {} },
    };
    std::vector<Renderable> renderables3{
        Renderable{ UnwrappedTileID{ 0, 0, 0 }, {} },
    };

    algorithm::ClipIDGenerator generator;
    generator.update<Renderable>({ renderables1.begin(), renderables1.end() });
    generator.update<Renderable>({ renderables2.begin(), renderables2.end() });
    generator.update<Renderable>({ renderables3.begin(), renderables3.end() });
    EXPECT_EQ(decltype(renderables1)({
                  Renderable{ UnwrappedTileID{ 1, 0, 0 }, ClipID{ "00000001", "00000001" } },
              }),
              renderables1);
    EXPECT_EQ(decltype(renderables2)({
                  Renderable{ UnwrappedTileID{ 0, 0, 0 }, ClipID{ "00000010", "00000010" } },
                  Renderable{ UnwrappedTileID{ 1, 0, 0 }, ClipID{ "00000011", "00000001" } },
              }),
              renderables2);
    EXPECT_EQ(decltype(renderables3)({
                  Renderable{ UnwrappedTileID{ 0, 0, 0 }, ClipID{ "00000100", "00000100" } },
              }),
              renderables3);

    const auto clipIDs = generator.getClipIDs();
    EXPECT_EQ(decltype(clipIDs)({
                  { UnwrappedTileID{ 0, 0, 0 }, ClipID{ "00000110", "00000110" } },
                  { UnwrappedTileID{ 1, 0, 0 }, ClipID{ "00000111", "00000101" } },
              }),
              clipIDs);
}
