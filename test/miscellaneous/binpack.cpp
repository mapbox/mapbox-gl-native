#include "../fixtures/util.hpp"

#include <mbgl/geometry/binpack.hpp>

#include <iosfwd>
#include <array>

namespace mbgl {
template <typename T> ::std::ostream& operator<<(::std::ostream& os, const Rect<T>& t) {
    return os << "Rect { " << t.x << ", " << t.y << ", " << t.w << ", " << t.h << " }";
}
}

TEST(BinPack, Allocating) {
    mbgl::BinPack<uint16_t> bin(128, 128);
    std::array<mbgl::Rect<uint16_t>, 4> rects;

    rects[0] = bin.allocate(32, 48);
    ASSERT_EQ(mbgl::Rect<uint16_t>(0, 0, 32, 48), rects[0]);
    rects[1] = bin.allocate(8, 17);
    ASSERT_EQ(mbgl::Rect<uint16_t>(32, 0, 8, 17), rects[1]);
    rects[2] = bin.allocate(8, 17);
    ASSERT_EQ(mbgl::Rect<uint16_t>(0, 48, 8, 17), rects[2]);

    bin.release(rects[0]);
    rects[0] = bin.allocate(32, 24);
    ASSERT_EQ(mbgl::Rect<uint16_t>(0, 0, 32, 24), rects[0]);
    rects[3] = bin.allocate(32, 24);
    ASSERT_EQ(mbgl::Rect<uint16_t>(32, 17, 32, 24), rects[3]);
}


TEST(BinPack, Full) {
    mbgl::BinPack<uint16_t> bin(128, 128);
    std::vector<mbgl::Rect<uint16_t>> rects;

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 256; i++) {
            auto rect = bin.allocate(8, 8);
            ASSERT_TRUE(rect.hasArea());
            rects.push_back(rect);
        }

        ASSERT_FALSE(bin.allocate(8, 8).hasArea());

        for (auto& rect: rects) {
            bin.release(rect);
        }
        rects.clear();
    }
}
