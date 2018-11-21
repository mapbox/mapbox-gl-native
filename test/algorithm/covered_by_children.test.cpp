#include <mbgl/test/util.hpp>

#include <mbgl/algorithm/covered_by_children.hpp>

#include <map>

using namespace mbgl;

using List = std::map<UnwrappedTileID, bool>;

TEST(CoveredByChildren, GrandChildren) {
    const List list {
        { UnwrappedTileID{ 0, 0, 0 }, true },
        // These grandchildren covers 1/0/0, but 1/0/0 only covers a quarter of
        // 0/0/0.
        { UnwrappedTileID{ 2, 0, 0 }, true },
        { UnwrappedTileID{ 2, 0, 1 }, true },
        { UnwrappedTileID{ 2, 1, 0 }, true },
        { UnwrappedTileID{ 2, 1, 1 }, true },
    };
    EXPECT_FALSE(algorithm::coveredByChildren(UnwrappedTileID{ 0, 0, 0 }, list));

    const List list2 {
        { UnwrappedTileID{ 0, 0, 0 }, true },

        // Children of 1/0/0
        { UnwrappedTileID{ 2, 0, 0 }, true },
        { UnwrappedTileID{ 2, 0, 1 }, true },
        { UnwrappedTileID{ 2, 1, 0 }, true },
        { UnwrappedTileID{ 2, 1, 1 }, true },

        // Children of 1/0/1
        { UnwrappedTileID{ 2, 0, 2 }, true },
        { UnwrappedTileID{ 2, 0, 3 }, true },
        { UnwrappedTileID{ 2, 1, 2 }, true },
        { UnwrappedTileID{ 2, 1, 3 }, true },

        // Children of 1/1/0
        { UnwrappedTileID{ 2, 2, 0 }, true },
        { UnwrappedTileID{ 2, 2, 1 }, true },
        { UnwrappedTileID{ 2, 3, 0 }, true },
        { UnwrappedTileID{ 2, 3, 1 }, true },

        // Children of 1/0/1
        { UnwrappedTileID{ 2, 2, 2 }, true },
        { UnwrappedTileID{ 2, 2, 3 }, true },
        { UnwrappedTileID{ 2, 3, 2 }, true },
        { UnwrappedTileID{ 2, 3, 3 }, true },
    };
    EXPECT_TRUE(algorithm::coveredByChildren(UnwrappedTileID{ 0, 0, 0 }, list2));
}

TEST(CoveredByChildren, NotCovered) {
    const List list1;
    EXPECT_FALSE(algorithm::coveredByChildren(UnwrappedTileID{ 0, 0, 0 }, list1));

    const List list2{
        { UnwrappedTileID{ 0, 0, 0 }, true },
        // 3 out of 4 child tiles
        { UnwrappedTileID{ 1, 0, 0 }, true },
        { UnwrappedTileID{ 1, 0, 1 }, true },
        // missing 1/1/0
        { UnwrappedTileID{ 1, 1, 1 }, true },
    };
    EXPECT_FALSE(algorithm::coveredByChildren(UnwrappedTileID{ 0, 0, 0 }, list2));

    const List list3{
        { UnwrappedTileID{ 0, 0, 0 }, true },
        // all four child tiles, with a different wrap index
        { UnwrappedTileID{ 1, { 1, 0, 0 } }, true },
        { UnwrappedTileID{ 1, { 1, 0, 1 } }, true },
        { UnwrappedTileID{ 1, { 1, 1, 0 } }, true },
        { UnwrappedTileID{ 1, { 1, 1, 1 } }, true },
    };
    EXPECT_FALSE(algorithm::coveredByChildren(UnwrappedTileID{ 0, 0, 0 }, list3));
}

TEST(CoveredByChildren, Covered) {
    const List list1{
        { UnwrappedTileID{ 0, 0, 0 }, true },
        // all four child tiles
        { UnwrappedTileID{ 1, 0, 0 }, true },
        { UnwrappedTileID{ 1, 0, 1 }, true },
        { UnwrappedTileID{ 1, 1, 0 }, true },
        { UnwrappedTileID{ 1, 1, 1 }, true },
    };
    EXPECT_TRUE(algorithm::coveredByChildren(UnwrappedTileID{ 0, 0, 0 }, list1));

    const List list2{
        { UnwrappedTileID{ 0, 0, 0 }, true },
        // missing 1/0/0
        { UnwrappedTileID{ 1, 0, 1 }, true },
        { UnwrappedTileID{ 1, 1, 0 }, true },
        { UnwrappedTileID{ 1, 1, 1 }, true },
        { UnwrappedTileID{ 2, 0, 0 }, true },
        { UnwrappedTileID{ 2, 0, 1 }, true },
        { UnwrappedTileID{ 2, 1, 0 }, true },
        { UnwrappedTileID{ 2, 1, 1 }, true },
    };
    EXPECT_TRUE(algorithm::coveredByChildren(UnwrappedTileID{ 0, 0, 0 }, list2));
    EXPECT_TRUE(algorithm::coveredByChildren(UnwrappedTileID{ 1, 0, 0 }, list2));
    EXPECT_FALSE(algorithm::coveredByChildren(UnwrappedTileID{ 2, 0, 0 }, list2));
}
