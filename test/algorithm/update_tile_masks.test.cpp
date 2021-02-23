#include <mbgl/test/util.hpp>
#include <mbgl/algorithm/update_tile_masks.hpp>

using namespace mbgl;

namespace {

class FakeTile {
public:
    FakeTile(TileMask mask_)
        : mask(std::move(mask_)) {
    }
    void setMask(TileMask mask_) {
        mask = std::move(mask_);
    }

    const bool usedByRenderedLayers = true;
    TileMask mask;
};

bool operator==(const FakeTile& lhs, const FakeTile& rhs) {
    return lhs.mask == rhs.mask;
}

} // namespace

void validate(std::map<UnwrappedTileID, FakeTile> expected) {
    auto actual = expected;
    std::for_each(actual.begin(), actual.end(),
                  [](auto& renderable) { renderable.second.mask.clear(); });

    algorithm::updateTileMasks(actual);
    EXPECT_EQ(expected, actual);
}

TEST(UpdateTileMasks, NoChildren) {
    validate({
        { UnwrappedTileID{ 0, 0, 0 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } }
    });

    validate({
        { UnwrappedTileID{ 4, 3, 8 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        { UnwrappedTileID{ 1, 0, 0 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 1, 1, 1 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        { UnwrappedTileID{ 1, 0, 0 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 2, 2, 3 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateTileMasks, ParentAndFourChildren) {
    validate({
        // Mask is empty (== not rendered!) because we have four covering children.
        { UnwrappedTileID{ 0, 0, 0 }, TileMask{} },
        // All four covering children
        { UnwrappedTileID{ 1, 0, 0 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 1, 0, 1 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 1, 1, 0 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 1, 1, 1 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateTileMasks, OneChild) {
    validate({
        { UnwrappedTileID{ 0, 0, 0 },
            // Only render the three children that aren't covering the other tile.
            TileMask{ CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                      CanonicalTileID{ 1, 1, 1 } } },
        { UnwrappedTileID{ 1, 0, 0 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateTileMasks, Complex) {
    validate({
        { UnwrappedTileID{ 0, 0, 0 },
            TileMask{ CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                      CanonicalTileID{ 2, 2, 3 }, CanonicalTileID{ 2, 3, 2 },
                      CanonicalTileID{ 3, 6, 7 }, CanonicalTileID{ 3, 7, 6 } } },
        { UnwrappedTileID{ 0, { 1, 0, 0 } }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 0, { 2, 2, 2 } }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 0, { 3, 7, 7 } }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 0, { 3, 6, 6 } }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        { UnwrappedTileID{ 0, 0, 0 },
            TileMask{ CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                      CanonicalTileID{ 1, 1, 1 }, CanonicalTileID{ 2, 0, 0 },
                      CanonicalTileID{ 2, 0, 1 }, CanonicalTileID{ 2, 1, 0 },
                      CanonicalTileID{ 3, 2, 3 }, CanonicalTileID{ 3, 3, 2 },
                      CanonicalTileID{ 3, 3, 3 }, CanonicalTileID{ 4, 4, 5 },
                      CanonicalTileID{ 4, 5, 4 }, CanonicalTileID{ 4, 5, 5 } } },
        { UnwrappedTileID{ 4, 4, 4 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        { UnwrappedTileID{ 12, 1028, 1456 },
            TileMask{ CanonicalTileID{ 1, 1, 1 }, CanonicalTileID{ 2, 3, 0 },
                      CanonicalTileID{ 2, 3, 1 } } },
        { UnwrappedTileID{ 13, 2056, 2912 },
            TileMask{ CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                      CanonicalTileID{ 1, 1, 1 } } },
        { UnwrappedTileID{ 13, 2056, 2913 },
            TileMask{ CanonicalTileID{ 1, 0, 0 }, CanonicalTileID{ 1, 1, 0 },
                      CanonicalTileID{ 1, 1, 1 } } },
        { UnwrappedTileID{ 14, 4112, 5824 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 14, 4112, 5827 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 14, 4114, 5824 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
        { UnwrappedTileID{ 14, 4114, 5825 }, TileMask{ CanonicalTileID{ 0, 0, 0 } } },
    });
}
