#include <iostream>
#include <mbgl/test/util.hpp>

#include <mbgl/util/string.hpp>

using namespace mbgl;

TEST(NumberConversions, number_to_string) {
    EXPECT_EQ("0", util::toString(0));
    EXPECT_EQ("10", util::toString(10));
    EXPECT_EQ("-10", util::toString(-10));
    EXPECT_EQ("10", util::toString(10.0));
    EXPECT_EQ("-10", util::toString(-10.0));

    EXPECT_EQ("1", util::toString(1));
    EXPECT_EQ("-1", util::toString(-1));
    EXPECT_EQ("32768", util::toString(32768));
    EXPECT_EQ("-32768", util::toString(-32768));

    EXPECT_EQ("1", util::toString(1.000000));
    EXPECT_EQ("-1", util::toString(-1.000000));
    EXPECT_EQ("32768", util::toString(32768.000000));
    EXPECT_EQ("-32768", util::toString(-32768.000000));

    // By default, ostringstream limits the output to 6 significant digits then trims any trailing zeros past the decimal point.
    EXPECT_EQ("1.01", util::toString(1.01));
    EXPECT_EQ("-1.01", util::toString(-1.01));
    EXPECT_EQ("32768", util::toString(32768.01));
    EXPECT_EQ("-32768", util::toString(-32768.01));

    EXPECT_EQ("1.01", util::toString(1.0100));
    EXPECT_EQ("-1.01", util::toString(-1.0100));
    EXPECT_EQ("32768", util::toString(32768.0100));
    EXPECT_EQ("-32768", util::toString(-32768.0100));

    EXPECT_EQ("1.12346", util::toString(1.123456));
    EXPECT_EQ("-1.12346", util::toString(-1.123456));
    EXPECT_EQ("32768.1", util::toString(32768.123456));
    EXPECT_EQ("-32768.1", util::toString(-32768.123456));

    EXPECT_EQ("1.12346", util::toString(1.123456789));
    EXPECT_EQ("-1.12346", util::toString(-1.123456789));
    EXPECT_EQ("32768.1", util::toString(32768.123456789));
    EXPECT_EQ("-32768.1", util::toString(-32768.123456789));

    EXPECT_EQ("3.14159", util::toString(3.1415926535897932385128089594061862044327426701784));
    EXPECT_EQ("-3.14159", util::toString(-3.1415926535897932385128089594061862044327426701784));

    EXPECT_EQ("123456789", util::toString(123456789));

    EXPECT_EQ("0.000053", util::toString((long double)5.32e-5));
    EXPECT_EQ("0.000000", util::toString((long double)5.32e-40));

    EXPECT_EQ("10", util::toString(unsigned(10)));
    EXPECT_EQ("10", util::toString(unsigned(10.0)));
}
