#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/util/utf.hpp>
#include <mbgl/util/platform.hpp>

using namespace mbgl;

TEST(TextConversions, to_upper) {
    EXPECT_EQ(std::string("STREET"), platform::uppercase("strEEt"));  // EN
    EXPECT_EQ(std::string("ROAD"), platform::uppercase("rOAd"));      // EN

    EXPECT_EQ(std::string("STRASSE"), platform::uppercase("straße")); // DE
    EXPECT_EQ(std::string("MASSE"), platform::uppercase("maße"));     // DE
    EXPECT_EQ(std::string("WEISSKOPFSEEADLER"), platform::uppercase("weißkopfseeadler")); // DE

    EXPECT_EQ(std::string("BÊNÇÃO"), platform::uppercase("bênção")); // PT
    EXPECT_EQ(std::string("AZƏRBAYCAN"), platform::uppercase("Azərbaycan")); // AZ
    EXPECT_EQ(std::string("ὈΔΥΣΣΕΎΣ"), platform::uppercase("Ὀδυσσεύς")); // GR
}


TEST(TextConversions, to_lower) {
    EXPECT_EQ(std::string("street"), platform::lowercase("strEEt"));  // EN
    EXPECT_EQ(std::string("road"), platform::lowercase("rOAd"));      // EN

    EXPECT_EQ(std::string("straße"), platform::lowercase("Straße"));   // DE
    EXPECT_EQ(std::string("strasse"), platform::lowercase("STRASSE")); // DE
    EXPECT_EQ(std::string("masse"), platform::lowercase("MASSE"));     // DE
    EXPECT_EQ(std::string("weisskopfseeadler"), platform::lowercase("weiSSkopfseeadler")); // DE

    EXPECT_EQ(std::string("bênção"), platform::lowercase("BÊNÇÃO")); // PT
    EXPECT_EQ(std::string("azərbaycan"), platform::lowercase("AZƏRBAYCAN")); // AZ

#if defined(__QT__)
    // https://bugreports.qt.io/browse/QTBUG-17337
    EXPECT_NE(std::string("ὀδυσσεύς"), platform::lowercase("ὈΔΥΣΣΕΎΣ")); // GR
#else
    EXPECT_EQ(std::string("ὀδυσσεύς"), platform::lowercase("ὈΔΥΣΣΕΎΣ")); // GR
#endif

}
