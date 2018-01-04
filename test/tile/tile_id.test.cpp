#include <mbgl/test/util.hpp>

#include <mbgl/tile/tile_id.hpp>

using namespace mbgl;

TEST(TileID, Canonical) {
    CanonicalTileID id_0_0_0(0, 0, 0);
    EXPECT_EQ(0u, id_0_0_0.z);
    EXPECT_EQ(0u, id_0_0_0.x);
    EXPECT_EQ(0u, id_0_0_0.y);

    EXPECT_TRUE(CanonicalTileID(4, 2, 3) == CanonicalTileID(4, 2, 3));
    EXPECT_FALSE(CanonicalTileID(4, 2, 3) != CanonicalTileID(4, 2, 3));
    EXPECT_TRUE(CanonicalTileID(4, 2, 3) != CanonicalTileID(5, 2, 3));
    EXPECT_FALSE(CanonicalTileID(4, 2, 3) == CanonicalTileID(5, 2, 3));
    EXPECT_TRUE(CanonicalTileID(4, 2, 3) != CanonicalTileID(4, 3, 3));
    EXPECT_FALSE(CanonicalTileID(4, 2, 3) == CanonicalTileID(4, 3, 3));
    EXPECT_TRUE(CanonicalTileID(4, 2, 3) != CanonicalTileID(4, 2, 4));
    EXPECT_FALSE(CanonicalTileID(4, 2, 3) == CanonicalTileID(4, 2, 4));
    EXPECT_TRUE(CanonicalTileID(5, 2, 3) != CanonicalTileID(4, 2, 3));
    EXPECT_FALSE(CanonicalTileID(5, 2, 3) == CanonicalTileID(4, 2, 3));
    EXPECT_TRUE(CanonicalTileID(4, 3, 3) != CanonicalTileID(4, 2, 3));
    EXPECT_FALSE(CanonicalTileID(4, 3, 3) == CanonicalTileID(4, 2, 3));
    EXPECT_TRUE(CanonicalTileID(4, 2, 4) != CanonicalTileID(4, 2, 3));
    EXPECT_FALSE(CanonicalTileID(4, 2, 4) == CanonicalTileID(4, 2, 3));

    EXPECT_TRUE(CanonicalTileID(0, 0, 0) < CanonicalTileID(1, 0, 0));
    EXPECT_FALSE(CanonicalTileID(1, 0, 0) < CanonicalTileID(0, 0, 0));
    EXPECT_TRUE(CanonicalTileID(4, 2, 3) < CanonicalTileID(4, 3, 2));
    EXPECT_FALSE(CanonicalTileID(4, 3, 2) < CanonicalTileID(4, 2, 3));
    EXPECT_FALSE(CanonicalTileID(5, 3, 6) < CanonicalTileID(5, 3, 6));

    const CanonicalTileID child(4, 2, 3);
    EXPECT_TRUE(child.isChildOf({ 3, 1, 1 }));
    EXPECT_TRUE(child.isChildOf({ 2, 0, 0 }));
    EXPECT_TRUE(child.isChildOf({ 1, 0, 0 }));
    EXPECT_TRUE(child.isChildOf({ 0, 0, 0 }));
    EXPECT_FALSE(child.isChildOf({ 5, 4, 6 }));
    EXPECT_FALSE(child.isChildOf({ 4, 2, 4 }));
    EXPECT_FALSE(child.isChildOf({ 3, 2, 2 }));
    EXPECT_FALSE(child.isChildOf({ 2, 1, 0 }));
    EXPECT_FALSE(child.isChildOf({ 1, 0, 1 }));

    EXPECT_EQ(0, 4294967295 >> 32);

    const CanonicalTileID maxChild(32, 4294967295, 4294967295);
    EXPECT_FALSE(maxChild.isChildOf({ 32, 4294967295, 4294967295 }));
    EXPECT_TRUE(maxChild.isChildOf({ 31, 2147483647, 2147483647 }));
    EXPECT_TRUE(maxChild.isChildOf({ 30, 1073741823, 1073741823 }));
    EXPECT_TRUE(maxChild.isChildOf({ 29, 536870911, 536870911 }));
    EXPECT_TRUE(maxChild.isChildOf({ 28, 268435455, 268435455 }));
    EXPECT_TRUE(maxChild.isChildOf({ 27, 134217727, 134217727 }));
    EXPECT_TRUE(maxChild.isChildOf({ 26, 67108863, 67108863 }));
    EXPECT_TRUE(maxChild.isChildOf({ 25, 33554431, 33554431 }));
    EXPECT_TRUE(maxChild.isChildOf({ 24, 16777215, 16777215 }));
    EXPECT_TRUE(maxChild.isChildOf({ 23, 8388607, 8388607 }));
    EXPECT_TRUE(maxChild.isChildOf({ 22, 4194303, 4194303 }));
    EXPECT_TRUE(maxChild.isChildOf({ 21, 2097151, 2097151 }));
    EXPECT_TRUE(maxChild.isChildOf({ 20, 1048575, 1048575 }));
    EXPECT_TRUE(maxChild.isChildOf({ 19, 524287, 524287 }));
    EXPECT_TRUE(maxChild.isChildOf({ 18, 262143, 262143 }));
    EXPECT_TRUE(maxChild.isChildOf({ 17, 131071, 131071 }));
    EXPECT_TRUE(maxChild.isChildOf({ 16, 65535, 65535 }));
    EXPECT_TRUE(maxChild.isChildOf({ 15, 32767, 32767 }));
    EXPECT_TRUE(maxChild.isChildOf({ 14, 16383, 16383 }));
    EXPECT_TRUE(maxChild.isChildOf({ 13, 8191, 8191 }));
    EXPECT_TRUE(maxChild.isChildOf({ 12, 4095, 4095 }));
    EXPECT_TRUE(maxChild.isChildOf({ 11, 2047, 2047 }));
    EXPECT_TRUE(maxChild.isChildOf({ 10, 1023, 1023 }));
    EXPECT_TRUE(maxChild.isChildOf({ 9, 511, 511 }));
    EXPECT_TRUE(maxChild.isChildOf({ 8, 255, 255 }));
    EXPECT_TRUE(maxChild.isChildOf({ 7, 127, 127 }));
    EXPECT_TRUE(maxChild.isChildOf({ 6, 63, 63 }));
    EXPECT_TRUE(maxChild.isChildOf({ 5, 31, 31 }));
    EXPECT_TRUE(maxChild.isChildOf({ 4, 15, 15 }));
    EXPECT_TRUE(maxChild.isChildOf({ 3, 7, 7 }));
    EXPECT_TRUE(maxChild.isChildOf({ 2, 3, 3 }));
    EXPECT_TRUE(maxChild.isChildOf({ 1, 1, 1 }));
    EXPECT_TRUE(maxChild.isChildOf({ 0, 0, 0 }));

    const CanonicalTileID scaleBase(4, 5, 13);
    EXPECT_EQ(CanonicalTileID(0, 0, 0), scaleBase.scaledTo(0));
    EXPECT_EQ(CanonicalTileID(1, 0, 1), scaleBase.scaledTo(1));
    EXPECT_EQ(CanonicalTileID(2, 1, 3), scaleBase.scaledTo(2));
    EXPECT_EQ(CanonicalTileID(3, 2, 6), scaleBase.scaledTo(3));
    EXPECT_EQ(CanonicalTileID(4, 5, 13), scaleBase.scaledTo(4));
    EXPECT_EQ(CanonicalTileID(5, 10, 26), scaleBase.scaledTo(5));
    EXPECT_EQ(CanonicalTileID(6, 20, 52), scaleBase.scaledTo(6));
    EXPECT_EQ(CanonicalTileID(7, 40, 104), scaleBase.scaledTo(7));
    EXPECT_EQ(CanonicalTileID(8, 80, 208), scaleBase.scaledTo(8));
    EXPECT_EQ(CanonicalTileID(9, 160, 416), scaleBase.scaledTo(9));
    EXPECT_EQ(CanonicalTileID(10, 320, 832), scaleBase.scaledTo(10));
    EXPECT_EQ(CanonicalTileID(11, 640, 1664), scaleBase.scaledTo(11));
    EXPECT_EQ(CanonicalTileID(12, 1280, 3328), scaleBase.scaledTo(12));
    EXPECT_EQ(CanonicalTileID(13, 2560, 6656), scaleBase.scaledTo(13));
    EXPECT_EQ(CanonicalTileID(14, 5120, 13312), scaleBase.scaledTo(14));
    EXPECT_EQ(CanonicalTileID(15, 10240, 26624), scaleBase.scaledTo(15));
    EXPECT_EQ(CanonicalTileID(16, 20480, 53248), scaleBase.scaledTo(16));
    EXPECT_EQ(CanonicalTileID(17, 40960, 106496), scaleBase.scaledTo(17));
    EXPECT_EQ(CanonicalTileID(18, 81920, 212992), scaleBase.scaledTo(18));
    EXPECT_EQ(CanonicalTileID(19, 163840, 425984), scaleBase.scaledTo(19));
    EXPECT_EQ(CanonicalTileID(20, 327680, 851968), scaleBase.scaledTo(20));
    EXPECT_EQ(CanonicalTileID(21, 655360, 1703936), scaleBase.scaledTo(21));
    EXPECT_EQ(CanonicalTileID(22, 1310720, 3407872), scaleBase.scaledTo(22));
    EXPECT_EQ(CanonicalTileID(23, 2621440, 6815744), scaleBase.scaledTo(23));
    EXPECT_EQ(CanonicalTileID(24, 5242880, 13631488), scaleBase.scaledTo(24));
    EXPECT_EQ(CanonicalTileID(25, 10485760, 27262976), scaleBase.scaledTo(25));
    EXPECT_EQ(CanonicalTileID(26, 20971520, 54525952), scaleBase.scaledTo(26));
    EXPECT_EQ(CanonicalTileID(27, 41943040, 109051904), scaleBase.scaledTo(27));
    EXPECT_EQ(CanonicalTileID(28, 83886080, 218103808), scaleBase.scaledTo(28));
    EXPECT_EQ(CanonicalTileID(29, 167772160, 436207616), scaleBase.scaledTo(29));
    EXPECT_EQ(CanonicalTileID(30, 335544320, 872415232), scaleBase.scaledTo(30));
    EXPECT_EQ(CanonicalTileID(31, 671088640, 1744830464), scaleBase.scaledTo(31));
    EXPECT_EQ(CanonicalTileID(32, 1342177280, 3489660928), scaleBase.scaledTo(32));

    EXPECT_EQ((std::array<CanonicalTileID, 4>{ {
                  { 5, 4, 6 }, { 5, 4, 7 }, { 5, 5, 6 }, { 5, 5, 7 },
              } }),
              CanonicalTileID(4, 2, 3).children());
    EXPECT_EQ((std::array<CanonicalTileID, 4>{ {
                  { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 },
              } }),
              CanonicalTileID(0, 0, 0).children());
}

TEST(TileID, Overscaled) {
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) == OverscaledTileID(4, 2, 3));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) != OverscaledTileID(4, 2, 3));
    EXPECT_TRUE(OverscaledTileID(4, 0, { 4, 2, 3 }) == OverscaledTileID(4, 2, 3));
    EXPECT_FALSE(OverscaledTileID(4, 0, { 4, 2, 3 }) != OverscaledTileID(4, 2, 3));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) == OverscaledTileID(4, 0, { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) != OverscaledTileID(4, 0, { 4, 2, 3 }));

    EXPECT_TRUE(OverscaledTileID(4, 2, 3) != OverscaledTileID(5, 0, { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) == OverscaledTileID(5, 0, { 4, 2, 3 }));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) != OverscaledTileID(6, 0, { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) == OverscaledTileID(6, 0, { 4, 2, 3 }));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) != OverscaledTileID(7, 0, { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) == OverscaledTileID(7, 0, { 4, 2, 3 }));

    EXPECT_TRUE(OverscaledTileID(4, 2, 3) != OverscaledTileID(4, 2, 4));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) == OverscaledTileID(4, 2, 4));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) != OverscaledTileID(4, 3, 3));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) == OverscaledTileID(4, 3, 3));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) != OverscaledTileID(5, 2, 3));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3) == OverscaledTileID(5, 2, 3));

    EXPECT_TRUE(OverscaledTileID(7, 0, { 4, 2, 3 }) == OverscaledTileID(7, 0, { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(7, 0, { 4, 2, 3 }) != OverscaledTileID(7, 0, { 4, 2, 3 }));

    EXPECT_FALSE(OverscaledTileID(4, 2, 3) < OverscaledTileID(4, 2, 3));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) < OverscaledTileID(5, 0, { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(5, 0, { 4, 2, 3 }) < OverscaledTileID(4, 2, 3));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) < OverscaledTileID(6, 0, { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(6, 0, { 4, 2, 3 }) < OverscaledTileID(4, 2, 3));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3) < OverscaledTileID(7,0,  { 4, 2, 3 }));
    EXPECT_FALSE(OverscaledTileID(7, 0, { 4, 2, 3 }) < OverscaledTileID(4, 2, 3));

    EXPECT_EQ(8u, OverscaledTileID(7, 0, { 4, 2, 3 }).overscaleFactor());
    EXPECT_EQ(4u, OverscaledTileID(6, 0, { 4, 2, 3 }).overscaleFactor());
    EXPECT_EQ(2u, OverscaledTileID(5, 0, { 4, 2, 3 }).overscaleFactor());
    EXPECT_EQ(1u, OverscaledTileID(4, 0, { 4, 2, 3 }).overscaleFactor());
    EXPECT_EQ(2147483648u, OverscaledTileID(31, 0, { 0, 0, 0 }).overscaleFactor());

    EXPECT_EQ(OverscaledTileID(0, 0, { 0, 0, 0 }), OverscaledTileID(4, 2, 3).scaledTo(0));
    EXPECT_EQ(OverscaledTileID(1, 0, { 1, 0, 0 }), OverscaledTileID(4, 2, 3).scaledTo(1));
    EXPECT_EQ(OverscaledTileID(2, 0, { 2, 0, 0 }), OverscaledTileID(4, 2, 3).scaledTo(2));
    EXPECT_EQ(OverscaledTileID(3, 0, { 3, 1, 1 }), OverscaledTileID(4, 2, 3).scaledTo(3));
    EXPECT_EQ(OverscaledTileID(4, 0, { 4, 2, 3 }), OverscaledTileID(4, 2, 3).scaledTo(4));
    EXPECT_EQ(OverscaledTileID(5, 0, { 4, 2, 3 }), OverscaledTileID(4, 2, 3).scaledTo(5));
    EXPECT_EQ(OverscaledTileID(6, 0, { 4, 2, 3 }), OverscaledTileID(4, 2, 3).scaledTo(6));
    EXPECT_EQ(OverscaledTileID(7, 0, { 4, 2, 3 }), OverscaledTileID(4, 2, 3).scaledTo(7));
    EXPECT_EQ(OverscaledTileID(8, 0, { 4, 2, 3 }), OverscaledTileID(4, 2, 3).scaledTo(8));
    EXPECT_EQ(OverscaledTileID(32, 0, { 4, 2, 3 }), OverscaledTileID(4, 2, 3).scaledTo(32));

    EXPECT_EQ(UnwrappedTileID(0, { 4, 2, 3 }), OverscaledTileID(4, 2, 3).toUnwrapped());
    EXPECT_EQ(UnwrappedTileID(-1, { 4, 2, 3 }), OverscaledTileID(4, -1, { 4, 2, 3 }).toUnwrapped());
    EXPECT_EQ(UnwrappedTileID(1, { 4, 2, 3 }), OverscaledTileID(4, 1, { 4, 2, 3 }).toUnwrapped());
    EXPECT_EQ(UnwrappedTileID(0, { 4, 2, 3 }), OverscaledTileID(5, 0, { 4, 2, 3 }).toUnwrapped());
    EXPECT_EQ(UnwrappedTileID(-1, { 4, 2, 3 }), OverscaledTileID(5, -1, { 4, 2, 3 }).toUnwrapped());
    EXPECT_EQ(UnwrappedTileID(1, { 4, 2, 3 }), OverscaledTileID(5, 1, { 4, 2, 3 }).toUnwrapped());

    EXPECT_FALSE(OverscaledTileID(2, 0, 0).isChildOf(OverscaledTileID(3, 1, 1)));
    EXPECT_FALSE(OverscaledTileID(3, 1, 1).isChildOf(OverscaledTileID(3, 1, 1)));
    EXPECT_TRUE(OverscaledTileID(4, 2, 3).isChildOf(OverscaledTileID(3, 1, 1)));
    EXPECT_TRUE(OverscaledTileID(5, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(3, 1, 1)));
    EXPECT_TRUE(OverscaledTileID(6, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(3, 1, 1)));
    EXPECT_TRUE(OverscaledTileID(7, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(3, 1, 1)));

    EXPECT_FALSE(OverscaledTileID(2, 0, 0).isChildOf(OverscaledTileID(5, 0, { 4, 2, 3 })));
    EXPECT_FALSE(OverscaledTileID(3, 1, 1).isChildOf(OverscaledTileID(5, 0, { 4, 2, 3 })));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3).isChildOf(OverscaledTileID(5, 0, { 4, 2, 3 })));
    EXPECT_FALSE(OverscaledTileID(5, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(5, 0, { 4, 2, 3 })));
    EXPECT_TRUE(OverscaledTileID(6, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(5, 0, { 4, 2, 3 })));
    EXPECT_TRUE(OverscaledTileID(7, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(5, 0, { 4, 2, 3 })));

    EXPECT_FALSE(OverscaledTileID(2, 0, 0).isChildOf(OverscaledTileID(6, 0, { 4, 2, 3 })));
    EXPECT_FALSE(OverscaledTileID(3, 1, 1).isChildOf(OverscaledTileID(6, 0, { 4, 2, 3 })));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3).isChildOf(OverscaledTileID(6, 0, { 4, 2, 3 })));
    EXPECT_FALSE(OverscaledTileID(5, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(6, 0, { 4, 2, 3 })));
    EXPECT_FALSE(OverscaledTileID(6, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(6, 0, { 4, 2, 3 })));
    EXPECT_TRUE(OverscaledTileID(7, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(6, 0, { 4, 2, 3 })));

    EXPECT_FALSE(OverscaledTileID(2, 0, 0).isChildOf(OverscaledTileID(5, 0, { 4, 0, 0 })));
    EXPECT_FALSE(OverscaledTileID(3, 1, 1).isChildOf(OverscaledTileID(5, 0, { 4, 0, 0 })));
    EXPECT_FALSE(OverscaledTileID(4, 2, 3).isChildOf(OverscaledTileID(5, 0, { 4, 0, 0 })));
    EXPECT_FALSE(OverscaledTileID(5, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(5, 0, { 4, 0, 0 })));
    EXPECT_FALSE(OverscaledTileID(6, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(5, 0, { 4, 0, 0 })));
    EXPECT_FALSE(OverscaledTileID(7, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(5, 0, { 4, 0, 0 })));

    EXPECT_FALSE(OverscaledTileID(4, 0, { 4, 2, 3 }).isChildOf(OverscaledTileID(5, 0, { 3, 1, 1 })));
}

TEST(TileID, Unwrapped) {
    UnwrappedTileID id_0_0_0(0, 0, 0);
    EXPECT_EQ(0u, id_0_0_0.canonical.z);
    EXPECT_EQ(0u, id_0_0_0.canonical.x);
    EXPECT_EQ(0u, id_0_0_0.canonical.y);
    EXPECT_EQ(0, id_0_0_0.wrap);

    UnwrappedTileID id_0_1_0(0, 1, 0);
    EXPECT_EQ(0u, id_0_1_0.canonical.z);
    EXPECT_EQ(0u, id_0_1_0.canonical.x);
    EXPECT_EQ(0u, id_0_1_0.canonical.y);
    EXPECT_EQ(1, id_0_1_0.wrap);

    UnwrappedTileID id_0_4_0(0, 4, 0);
    EXPECT_EQ(0u, id_0_4_0.canonical.z);
    EXPECT_EQ(0u, id_0_4_0.canonical.x);
    EXPECT_EQ(0u, id_0_4_0.canonical.y);
    EXPECT_EQ(4, id_0_4_0.wrap);

    UnwrappedTileID id_0_n1_0(0, -1, 0);
    EXPECT_EQ(0u, id_0_n1_0.canonical.z);
    EXPECT_EQ(0u, id_0_n1_0.canonical.x);
    EXPECT_EQ(0u, id_0_n1_0.canonical.y);
    EXPECT_EQ(-1, id_0_n1_0.wrap);

    UnwrappedTileID id_0_0_1(0, 0, 1);
    EXPECT_EQ(0u, id_0_0_1.canonical.z);
    EXPECT_EQ(0u, id_0_0_1.canonical.x);
    EXPECT_EQ(0u, id_0_0_1.canonical.y);
    EXPECT_EQ(0, id_0_0_1.wrap);

    UnwrappedTileID id_0_0_n1(0, 0, -1);
    EXPECT_EQ(0u, id_0_0_n1.canonical.z);
    EXPECT_EQ(0u, id_0_0_n1.canonical.x);
    EXPECT_EQ(0u, id_0_0_n1.canonical.y);
    EXPECT_EQ(0, id_0_0_n1.wrap);

    UnwrappedTileID id_18_262143_0(18, 262143, 0);
    EXPECT_EQ(18u, id_18_262143_0.canonical.z);
    EXPECT_EQ(262143u, id_18_262143_0.canonical.x);
    EXPECT_EQ(0u, id_18_262143_0.canonical.y);
    EXPECT_EQ(0, id_18_262143_0.wrap);

    UnwrappedTileID id_18_262144_0(18, 262144, 0);
    EXPECT_EQ(18u, id_18_262144_0.canonical.z);
    EXPECT_EQ(0u, id_18_262144_0.canonical.x);
    EXPECT_EQ(0u, id_18_262144_0.canonical.y);
    EXPECT_EQ(1, id_18_262144_0.wrap);

    UnwrappedTileID a(2, 4, 3);
    UnwrappedTileID b(a);
    EXPECT_EQ(2u, b.canonical.z);
    EXPECT_EQ(0u, b.canonical.x);
    EXPECT_EQ(3u, b.canonical.y);
    EXPECT_EQ(1, b.wrap);

    EXPECT_TRUE(UnwrappedTileID(0, 0, 0) < UnwrappedTileID(1, 0, 0));
    EXPECT_TRUE(UnwrappedTileID(1, 0, 0) < UnwrappedTileID(0, 1, 0));
    EXPECT_FALSE(UnwrappedTileID(0, 1, 0) < UnwrappedTileID(1, 0, 0));
    EXPECT_FALSE(UnwrappedTileID(5, 3, 6) < UnwrappedTileID(5, 3, 6));

    EXPECT_EQ(OverscaledTileID(4, 0, { 4, 2, 3 }), UnwrappedTileID(4, 2, 3).overscaleTo(4));
    EXPECT_EQ(OverscaledTileID(5, 0, { 4, 2, 3 }), UnwrappedTileID(4, 2, 3).overscaleTo(5));
    EXPECT_EQ(OverscaledTileID(6, 0, { 4, 2, 3 }), UnwrappedTileID(4, 2, 3).overscaleTo(6));
    EXPECT_EQ(OverscaledTileID(32, 0, { 4, 2, 3 }), UnwrappedTileID(4, 2, 3).overscaleTo(32));

    EXPECT_EQ(UnwrappedTileID(-1, { 1, 0, 0 }), UnwrappedTileID(1, -2, 0));
    EXPECT_EQ(UnwrappedTileID(-1, { 1, 0, 1 }), UnwrappedTileID(1, -2, 1));
    EXPECT_EQ(UnwrappedTileID(-1, { 1, 1, 0 }), UnwrappedTileID(1, -1, 0));
    EXPECT_EQ(UnwrappedTileID(-1, { 1, 1, 1 }), UnwrappedTileID(1, -1, 1));
    EXPECT_EQ(UnwrappedTileID(-1, { 0, 0, 0 }), UnwrappedTileID(0, -1, 0));

    EXPECT_EQ((std::array<UnwrappedTileID, 4>{ {
                  { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 },
              } }),
              UnwrappedTileID(0, 0, 0).children());
    EXPECT_EQ((std::array<UnwrappedTileID, 4>{ {
                  { 1, -2, 0 }, { 1, -2, 1 }, { 1, -1, 0 }, { 1, -1, 1 },
              } }),
              UnwrappedTileID(0, -1, 0).children());
}
