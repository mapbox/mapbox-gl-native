#include <mbgl/test/util.hpp>

#include <mbgl/algorithm/update_tile_masks.hpp>

using namespace mbgl;

namespace {

class MaskedRenderable {
public:
    MaskedRenderable(const UnwrappedTileID& id_, TileMask&& mask_)
        : id(id_), mask(std::move(mask_)) {
    }

    UnwrappedTileID id;
    TileMask mask;
    bool used = true;

    void setMask(TileMask&& mask_) {
        mask = std::move(mask_);
    }
};

bool operator==(const MaskedRenderable& lhs, const MaskedRenderable& rhs) {
    return lhs.id == rhs.id && lhs.mask == rhs.mask;
}

::std::ostream& operator<<(::std::ostream& os, const MaskedRenderable& rhs) {
    os << "MaskedRenderable{ " << rhs.id << ", { ";
    bool first = true;
    for (auto& id : rhs.mask) {
        if (!first) {
            os << ", ";
        } else {
            first = false;
        }
        os << id;
    }
    return os << " } }";
}

} // namespace

void validate(const std::vector<MaskedRenderable> expected) {
    std::vector<MaskedRenderable> actual = expected;
    std::for_each(actual.begin(), actual.end(),
                  [](auto& renderable) { renderable.mask.clear(); });
    algorithm::updateTileMasks<MaskedRenderable>({ actual.begin(), actual.end() });
    EXPECT_EQ(expected, actual);
}

TEST(UpdateTileMasks, NoChildren) {
    validate({
        MaskedRenderable{ UnwrappedTileID{ 0, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        MaskedRenderable{ UnwrappedTileID{ 4, 3, 8 }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        MaskedRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 1, 1, 1 }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        MaskedRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 2, 2, 3 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateTileMasks, ParentAndFourChildren) {
    validate({
        // Mask is empty (== not rendered!) because we have four covering children.
        MaskedRenderable{ UnwrappedTileID{ 0, 0, 0 }, {} },
        // All four covering children
        MaskedRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 1, 0, 1 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 1, 1, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 1, 1, 1 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateTileMasks, OneChild) {
    validate({
        MaskedRenderable{ UnwrappedTileID{ 0, 0, 0 },
                          // Only render the three children that aren't covering the other tile.
                          { CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                            CanonicalTileID{ 1, 1, 1 } } },
        MaskedRenderable{ UnwrappedTileID{ 1, 0, 0 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}

TEST(UpdateTileMasks, Complex) {
    validate({
        MaskedRenderable{ UnwrappedTileID{ 0, 0, 0 },
                          { CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                            CanonicalTileID{ 2, 2, 3 }, CanonicalTileID{ 2, 3, 2 },
                            CanonicalTileID{ 3, 6, 7 }, CanonicalTileID{ 3, 7, 6 } } },
        MaskedRenderable{ UnwrappedTileID{ 0, { 1, 0, 0 } }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 0, { 2, 2, 2 } }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 0, { 3, 7, 7 } }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 0, { 3, 6, 6 } }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        MaskedRenderable{ UnwrappedTileID{ 0, 0, 0 },
                          { CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                            CanonicalTileID{ 1, 1, 1 }, CanonicalTileID{ 2, 0, 0 },
                            CanonicalTileID{ 2, 0, 1 }, CanonicalTileID{ 2, 1, 0 },
                            CanonicalTileID{ 3, 2, 3 }, CanonicalTileID{ 3, 3, 2 },
                            CanonicalTileID{ 3, 3, 3 }, CanonicalTileID{ 4, 4, 5 },
                            CanonicalTileID{ 4, 5, 4 }, CanonicalTileID{ 4, 5, 5 } } },
        MaskedRenderable{ UnwrappedTileID{ 4, 4, 4 }, { CanonicalTileID{ 0, 0, 0 } } },
    });

    validate({
        MaskedRenderable{ UnwrappedTileID{ 12, 1028, 1456 },
                          { CanonicalTileID{ 1, 1, 1 }, CanonicalTileID{ 2, 3, 0 },
                            CanonicalTileID{ 2, 3, 1 } } },
        MaskedRenderable{ UnwrappedTileID{ 13, 2056, 2912 },
                          { CanonicalTileID{ 1, 0, 1 }, CanonicalTileID{ 1, 1, 0 },
                            CanonicalTileID{ 1, 1, 1 } } },
        MaskedRenderable{ UnwrappedTileID{ 13, 2056, 2913 },
                          { CanonicalTileID{ 1, 0, 0 }, CanonicalTileID{ 1, 1, 0 },
                            CanonicalTileID{ 1, 1, 1 } } },
        MaskedRenderable{ UnwrappedTileID{ 14, 4112, 5824 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 14, 4112, 5827 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 14, 4114, 5824 }, { CanonicalTileID{ 0, 0, 0 } } },
        MaskedRenderable{ UnwrappedTileID{ 14, 4114, 5825 }, { CanonicalTileID{ 0, 0, 0 } } },
    });
}
