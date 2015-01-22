#include <iostream>
#include "../fixtures/util.hpp"

#include <mbgl/util/utf.hpp>
#include <mbgl/platform/platform.hpp>

using namespace mbgl;

TEST(TextConversions, to_upper) {
    EXPECT_EQ(std::string("STREET"), platform::uppercase("strEEt"));  // EN
    EXPECT_EQ(std::string("ROAD"), platform::uppercase("rOAd"));      // EN

    EXPECT_EQ(std::string("STRASSE"), platform::uppercase("straße")); // DE
    EXPECT_EQ(std::string("MASSE"), platform::uppercase("maße"));     // DE
    EXPECT_EQ(std::string("WEISSKOPFSEEADLER"), platform::uppercase("weißkopfseeadler")); // DE

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

    EXPECT_EQ(std::string("azərbaycan"), platform::lowercase("AZƏRBAYCAN")); // AZ
    EXPECT_EQ(std::string("ὀδυσσεύς"), platform::lowercase("ὈΔΥΣΣΕΎΣ")); // GR

}
