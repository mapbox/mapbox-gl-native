#include <iostream>
#include "../fixtures/util.hpp"

#include <mbgl/style/value.hpp>
#include <mbgl/style/value_comparison.hpp>

using namespace mbgl;

TEST(Variant, toString) {
    EXPECT_EQ("lord", toString( (std::string)"lord" ));
    EXPECT_EQ("36", toString( (uint64_t)36 ));
    EXPECT_EQ("-239", toString( (int64_t)-239 ));
    EXPECT_EQ("0", toString( (int64_t)0 ));
    EXPECT_EQ("3.14159268", toString( (double)3.14159268 ));
    EXPECT_EQ("3.1415926535897931", toString( (double)3.1415926535897931 ));
    EXPECT_EQ("true", toString( true ));
    EXPECT_EQ("false", toString( false ));
}

TEST(Variant, RelaxedEquality) {
    // Compare to bool
    EXPECT_TRUE(util::relaxed_equal(bool(false), bool(false)));
    EXPECT_TRUE(util::relaxed_equal(bool(true), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), int64_t(0)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), uint64_t(0)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), double(0)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), std::string("0")));
    EXPECT_FALSE(util::relaxed_equal(bool(false), std::string("")));
    EXPECT_FALSE(util::relaxed_equal(bool(false), int64_t(386)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), int64_t(-7042)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), double(3.145)));
    EXPECT_FALSE(util::relaxed_equal(bool(false), std::string("386")));
    EXPECT_FALSE(util::relaxed_equal(bool(false), std::string("test")));
    EXPECT_FALSE(util::relaxed_equal(bool(true), int64_t(1)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), uint64_t(1)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), double(1)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), std::string("1")));
    EXPECT_FALSE(util::relaxed_equal(bool(true), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), int64_t(386)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), int64_t(-7042)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), double(3.145)));
    EXPECT_FALSE(util::relaxed_equal(bool(true), std::string("386")));
    EXPECT_FALSE(util::relaxed_equal(bool(true), std::string("test")));
    EXPECT_FALSE(util::relaxed_equal(bool(true), std::string("")));

    // Compare to int64_t
    EXPECT_TRUE(util::relaxed_equal(int64_t(386), int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal(int64_t(-7042), int64_t(-7042)));
    EXPECT_TRUE(util::relaxed_equal(int64_t(386), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal(int64_t(386), double(386)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(386), std::string("386")));
    EXPECT_FALSE(util::relaxed_equal(int64_t(1), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(0), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(386), int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(-7042), int64_t(-7043)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(386), uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(386), double(387)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(386), std::string("387")));
    EXPECT_FALSE(util::relaxed_equal(int64_t(1), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(0), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(int64_t(1), std::string("true")));
    EXPECT_FALSE(util::relaxed_equal(int64_t(0), std::string("false")));
    EXPECT_FALSE(util::relaxed_equal(int64_t(1), std::string("false")));
    EXPECT_FALSE(util::relaxed_equal(int64_t(0), std::string("true")));

    // Compare to uint64_t
    EXPECT_TRUE(util::relaxed_equal(uint64_t(386), int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal(uint64_t(386), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal(uint64_t(0), uint64_t(0)));
    EXPECT_TRUE(util::relaxed_equal(uint64_t(386), double(386)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(386), std::string("386")));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(1), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(0), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(386), int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(386), uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(0), uint64_t(1)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(386), double(387)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(386), std::string("387")));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(1), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(0), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(1), std::string("true")));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(0), std::string("false")));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(1), std::string("false")));
    EXPECT_FALSE(util::relaxed_equal(uint64_t(0), std::string("true")));

    // Compare to double
    EXPECT_TRUE(util::relaxed_equal(double(3.159385), double(3.159385)));
    EXPECT_TRUE(util::relaxed_equal(double(386), int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal(double(386), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal(double(0), uint64_t(0)));
    EXPECT_TRUE(util::relaxed_equal(double(386), double(386)));
    EXPECT_FALSE(util::relaxed_equal(double(386), std::string("386")));
    EXPECT_FALSE(util::relaxed_equal(double(1), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(double(0), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(double(3.159385), double(3.159383)));
    EXPECT_FALSE(util::relaxed_equal(double(386), int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(double(386), uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(double(0), uint64_t(1)));
    EXPECT_FALSE(util::relaxed_equal(double(386), double(387)));
    EXPECT_FALSE(util::relaxed_equal(double(386), std::string("387")));
    EXPECT_FALSE(util::relaxed_equal(double(1), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(double(0), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(double(1), std::string("true")));
    EXPECT_FALSE(util::relaxed_equal(double(1), std::string("TRue")));
    EXPECT_FALSE(util::relaxed_equal(double(0), std::string("false")));
    EXPECT_FALSE(util::relaxed_equal(double(0), std::string("FALSe")));
    EXPECT_FALSE(util::relaxed_equal(double(0), std::string("null")));
    EXPECT_FALSE(util::relaxed_equal(double(0), std::string("Null")));
    EXPECT_FALSE(util::relaxed_equal(double(1), std::string("false")));
    EXPECT_FALSE(util::relaxed_equal(double(1), std::string("False")));
    EXPECT_FALSE(util::relaxed_equal(double(1), std::string("null")));
    EXPECT_FALSE(util::relaxed_equal(double(1), std::string("NULL")));
    EXPECT_FALSE(util::relaxed_equal(double(0), std::string("true")));
    EXPECT_FALSE(util::relaxed_equal(double(0), std::string("TRUE")));

    // Compare to string
    EXPECT_TRUE(util::relaxed_equal(std::string("lord"), std::string("lord")));
    EXPECT_FALSE(util::relaxed_equal(std::string("386"), int64_t(386)));
    EXPECT_FALSE(util::relaxed_equal(std::string("-386"), int64_t(-386)));
    EXPECT_FALSE(util::relaxed_equal(std::string("386"), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_equal(std::string("386.36"), double(386.36)));
    EXPECT_FALSE(util::relaxed_equal(std::string(""), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("1"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string("lord"), std::string("baron")));
    EXPECT_FALSE(util::relaxed_equal(std::string("386"), int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(std::string("-386"), int64_t(-387)));
    EXPECT_FALSE(util::relaxed_equal(std::string("386"), uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal(std::string("386.36"), double(386.37)));
    EXPECT_FALSE(util::relaxed_equal(std::string("some string"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string("true"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string("True"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string("false"), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("fAlse"), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("null"), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("NULl"), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("true"), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("TRUE"), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("some string"), bool(false)));
    EXPECT_FALSE(util::relaxed_equal(std::string("false"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string("FALSE"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string("null"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string("NULl"), bool(true)));
    EXPECT_FALSE(util::relaxed_equal(std::string(""), bool(true)));
}

TEST(Variant, RelaxedGreater) {
    EXPECT_TRUE(util::relaxed_greater(std::string("387"), std::string("386")));
    EXPECT_FALSE(util::relaxed_greater(uint64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_greater(int64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_greater(double(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_greater(bool(true), std::string("0")));

    EXPECT_FALSE(util::relaxed_greater(std::string("387"), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_greater(uint64_t(387), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_greater(int64_t(387), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_greater(double(387), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), uint64_t(0)));

    EXPECT_FALSE(util::relaxed_greater(std::string("387"), int64_t(386)));
    EXPECT_TRUE(util::relaxed_greater(uint64_t(387), int64_t(386)));
    EXPECT_TRUE(util::relaxed_greater(int64_t(387), int64_t(386)));
    EXPECT_TRUE(util::relaxed_greater(double(387), int64_t(386)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), int64_t(0)));

    EXPECT_FALSE(util::relaxed_greater(std::string("387"), double(386.7)));
    EXPECT_TRUE(util::relaxed_greater(uint64_t(387), double(386.7)));
    EXPECT_TRUE(util::relaxed_greater(int64_t(387), double(386.7)));
    EXPECT_TRUE(util::relaxed_greater(double(387), double(386.7)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), double(0)));

    EXPECT_FALSE(util::relaxed_greater(std::string("387"), bool(true)));
    EXPECT_FALSE(util::relaxed_greater(std::string("387"), bool(false)));
    EXPECT_FALSE(util::relaxed_greater(uint64_t(387), bool(false)));
    EXPECT_FALSE(util::relaxed_greater(int64_t(387), bool(false)));
    EXPECT_FALSE(util::relaxed_greater(double(387), bool(false)));
    EXPECT_FALSE(util::relaxed_greater(double(387), bool(false)));
    EXPECT_FALSE(util::relaxed_greater(uint64_t(387), bool(true)));
    EXPECT_FALSE(util::relaxed_greater(int64_t(387), bool(true)));
    EXPECT_FALSE(util::relaxed_greater(double(387), bool(true)));
    EXPECT_FALSE(util::relaxed_greater(double(387), bool(true)));

    EXPECT_FALSE(util::relaxed_greater(bool(true), double(0)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), double(0.5)));
    EXPECT_TRUE(util::relaxed_greater(bool(true), bool(false)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), bool(true)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), int64_t(0)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), uint64_t(0)));




    EXPECT_FALSE(util::relaxed_greater(std::string("387"), std::string("388")));
    EXPECT_FALSE(util::relaxed_greater(uint64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_greater(int64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_greater(double(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_greater(bool(true), std::string("388")));

    EXPECT_FALSE(util::relaxed_greater(std::string("387"), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(uint64_t(387), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(int64_t(387), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(double(387), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), uint64_t(388)));

    EXPECT_FALSE(util::relaxed_greater(std::string("387"), int64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(uint64_t(387), int64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(int64_t(387), int64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(double(387), int64_t(388)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), int64_t(388)));

    EXPECT_FALSE(util::relaxed_greater(std::string("387"), double(388.7)));
    EXPECT_FALSE(util::relaxed_greater(uint64_t(387), double(388.7)));
    EXPECT_FALSE(util::relaxed_greater(int64_t(387), double(388.7)));
    EXPECT_FALSE(util::relaxed_greater(double(387), double(388.7)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), double(388)));
    EXPECT_FALSE(util::relaxed_greater(bool(true), bool(true)));
    EXPECT_FALSE(util::relaxed_greater(bool(false), bool(false)));
}



TEST(Variant, RelaxedGreaterEqual) {
    EXPECT_TRUE(util::relaxed_greater_equal(std::string("387"), std::string("386")));
    EXPECT_TRUE(util::relaxed_greater_equal(std::string("387"), std::string("387")));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), std::string("387")));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), std::string("387")));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), std::string("387")));
    EXPECT_FALSE(util::relaxed_greater_equal(bool(true), std::string("0")));

    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), uint64_t(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(uint64_t(387), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_greater_equal(uint64_t(387), uint64_t(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(int64_t(387), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_greater_equal(int64_t(387), uint64_t(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(double(387), uint64_t(386)));
    EXPECT_TRUE(util::relaxed_greater_equal(double(387), uint64_t(387)));
    EXPECT_FALSE(util::relaxed_greater_equal(bool(true), uint64_t(0)));

    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), int64_t(386)));
    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), int64_t(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(uint64_t(387), int64_t(386)));
    EXPECT_TRUE(util::relaxed_greater_equal(uint64_t(387), int64_t(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(int64_t(387), int64_t(386)));
    EXPECT_TRUE(util::relaxed_greater_equal(int64_t(387), int64_t(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(double(387), int64_t(386)));
    EXPECT_TRUE(util::relaxed_greater_equal(double(387), int64_t(387)));
    EXPECT_FALSE(util::relaxed_greater_equal(bool(true), int64_t(0)));

    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), double(386.7)));
    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), double(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(uint64_t(387), double(386.7)));
    EXPECT_TRUE(util::relaxed_greater_equal(uint64_t(387), double(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(int64_t(387), double(386.7)));
    EXPECT_TRUE(util::relaxed_greater_equal(int64_t(387), double(387)));
    EXPECT_TRUE(util::relaxed_greater_equal(double(387), double(386.7)));
    EXPECT_TRUE(util::relaxed_greater_equal(double(387), double(387)));
    EXPECT_FALSE(util::relaxed_greater_equal(bool(true), double(0)));

    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), bool(false)));
    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), bool(true)));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), bool(false)));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), bool(true)));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), bool(false)));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), bool(true)));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), bool(false)));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), bool(true)));
    EXPECT_TRUE(util::relaxed_greater_equal(bool(true), bool(false)));
    EXPECT_TRUE(util::relaxed_greater_equal(bool(true), bool(true)));




    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), std::string("388")));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), std::string("388")));

    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), uint64_t(388)));

    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), int64_t(388)));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), int64_t(388)));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), int64_t(388)));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), int64_t(388)));

    EXPECT_FALSE(util::relaxed_greater_equal(std::string("387"), double(388.7)));
    EXPECT_FALSE(util::relaxed_greater_equal(uint64_t(387), double(388.7)));
    EXPECT_FALSE(util::relaxed_greater_equal(int64_t(387), double(388.7)));
    EXPECT_FALSE(util::relaxed_greater_equal(double(387), double(388.7)));
}


TEST(Variant, RelaxedLess) {
    EXPECT_TRUE(util::relaxed_less(std::string("387"), std::string("388")));
    EXPECT_FALSE(util::relaxed_less(uint64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_less(int64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_less(double(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_less(bool(false), std::string("388")));
    EXPECT_FALSE(util::relaxed_less(bool(false), std::string("1")));

    EXPECT_FALSE(util::relaxed_less(std::string("387"), uint64_t(388)));
    EXPECT_TRUE(util::relaxed_less(uint64_t(387), uint64_t(388)));
    EXPECT_TRUE(util::relaxed_less(int64_t(387), uint64_t(388)));
    EXPECT_TRUE(util::relaxed_less(double(387), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_less(bool(false), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_less(bool(false), uint64_t(1)));

    EXPECT_FALSE(util::relaxed_less(std::string("387"), int64_t(388)));
    EXPECT_TRUE(util::relaxed_less(uint64_t(387), int64_t(388)));
    EXPECT_TRUE(util::relaxed_less(int64_t(387), int64_t(388)));
    EXPECT_TRUE(util::relaxed_less(double(387), int64_t(388)));
    EXPECT_FALSE(util::relaxed_less(bool(false), int64_t(388)));
    EXPECT_FALSE(util::relaxed_less(bool(false), int64_t(1)));

    EXPECT_FALSE(util::relaxed_less(std::string("387"), double(387.7)));
    EXPECT_TRUE(util::relaxed_less(uint64_t(387), double(387.7)));
    EXPECT_TRUE(util::relaxed_less(int64_t(387), double(387.7)));
    EXPECT_TRUE(util::relaxed_less(double(387), double(387.7)));
    EXPECT_FALSE(util::relaxed_less(bool(false), double(387)));
    EXPECT_FALSE(util::relaxed_less(bool(false), double(1)));

    EXPECT_FALSE(util::relaxed_less(std::string("0"), bool(true)));
    EXPECT_FALSE(util::relaxed_less(uint64_t(0), bool(true)));
    EXPECT_FALSE(util::relaxed_less(int64_t(0), bool(true)));
    EXPECT_FALSE(util::relaxed_less(double(0), bool(true)));
    EXPECT_TRUE(util::relaxed_less(bool(false), bool(true)));

    EXPECT_FALSE(util::relaxed_less(bool(true), std::string("386")));
    EXPECT_FALSE(util::relaxed_less(bool(true), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_less(bool(true), int64_t(386)));
    EXPECT_FALSE(util::relaxed_less(bool(true), double(386)));




    EXPECT_FALSE(util::relaxed_less(std::string("387"), std::string("386")));
    EXPECT_FALSE(util::relaxed_less(uint64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_less(int64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_less(double(387), std::string("386")));

    EXPECT_FALSE(util::relaxed_less(std::string("387"), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_less(uint64_t(387), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_less(int64_t(387), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_less(double(387), uint64_t(386)));

    EXPECT_FALSE(util::relaxed_less(std::string("387"), int64_t(386)));
    EXPECT_FALSE(util::relaxed_less(uint64_t(387), int64_t(386)));
    EXPECT_FALSE(util::relaxed_less(int64_t(387), int64_t(386)));
    EXPECT_FALSE(util::relaxed_less(double(387), int64_t(386)));

    EXPECT_FALSE(util::relaxed_less(std::string("387"), double(386.7)));
    EXPECT_FALSE(util::relaxed_less(uint64_t(387), double(386.7)));
    EXPECT_FALSE(util::relaxed_less(int64_t(387), double(386.7)));
    EXPECT_FALSE(util::relaxed_less(double(387), double(386.7)));

    EXPECT_FALSE(util::relaxed_less(std::string("387"), bool(true)));
    EXPECT_FALSE(util::relaxed_less(uint64_t(387), bool(true)));
    EXPECT_FALSE(util::relaxed_less(int64_t(387), bool(true)));
    EXPECT_FALSE(util::relaxed_less(double(387), bool(true)));
    EXPECT_FALSE(util::relaxed_less(bool(true), bool(true)));
    EXPECT_FALSE(util::relaxed_less(bool(false), bool(false)));
}


TEST(Variant, RelaxedLessEqual) {
    EXPECT_TRUE(util::relaxed_less_equal(std::string("387"), std::string("388")));
    EXPECT_TRUE(util::relaxed_less_equal(std::string("387"), std::string("387")));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(387), std::string("387")));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(387), std::string("387")));
    EXPECT_FALSE(util::relaxed_less_equal(double(387), std::string("388")));
    EXPECT_FALSE(util::relaxed_less_equal(double(387), std::string("387")));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), std::string("388")));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), std::string("1")));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), std::string("0")));

    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), uint64_t(387)));
    EXPECT_TRUE(util::relaxed_less_equal(uint64_t(387), uint64_t(388)));
    EXPECT_TRUE(util::relaxed_less_equal(uint64_t(387), uint64_t(387)));
    EXPECT_TRUE(util::relaxed_less_equal(int64_t(387), uint64_t(388)));
    EXPECT_TRUE(util::relaxed_less_equal(int64_t(387), uint64_t(387)));
    EXPECT_TRUE(util::relaxed_less_equal(double(387), uint64_t(388)));
    EXPECT_TRUE(util::relaxed_less_equal(double(387), uint64_t(387)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), uint64_t(388)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), uint64_t(1)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), uint64_t(0)));

    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), int64_t(388)));
    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), int64_t(387)));
    EXPECT_TRUE(util::relaxed_less_equal(uint64_t(387), int64_t(388)));
    EXPECT_TRUE(util::relaxed_less_equal(uint64_t(387), int64_t(387)));
    EXPECT_TRUE(util::relaxed_less_equal(int64_t(387), int64_t(388)));
    EXPECT_TRUE(util::relaxed_less_equal(int64_t(387), int64_t(387)));
    EXPECT_TRUE(util::relaxed_less_equal(double(387), int64_t(388)));
    EXPECT_TRUE(util::relaxed_less_equal(double(387), int64_t(387)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), int64_t(388)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), int64_t(1)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), int64_t(0)));

    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), double(387.7)));
    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), double(387)));
    EXPECT_TRUE(util::relaxed_less_equal(uint64_t(387), double(387.7)));
    EXPECT_TRUE(util::relaxed_less_equal(uint64_t(387), double(387)));
    EXPECT_TRUE(util::relaxed_less_equal(int64_t(387), double(387.7)));
    EXPECT_TRUE(util::relaxed_less_equal(int64_t(387), double(387)));
    EXPECT_TRUE(util::relaxed_less_equal(double(387), double(387.7)));
    EXPECT_TRUE(util::relaxed_less_equal(double(387), double(387)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), double(387)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), double(1)));
    EXPECT_FALSE(util::relaxed_less_equal(bool(false), double(0)));

    EXPECT_FALSE(util::relaxed_less_equal(std::string("0"), bool(true)));
    EXPECT_FALSE(util::relaxed_less_equal(std::string("0"), bool(false)));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(0), bool(true)));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(0), bool(false)));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(0), bool(true)));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(0), bool(false)));
    EXPECT_FALSE(util::relaxed_less_equal(double(0), bool(true)));
    EXPECT_FALSE(util::relaxed_less_equal(double(0), bool(false)));
    EXPECT_TRUE(util::relaxed_less_equal(bool(false), bool(true)));
    EXPECT_TRUE(util::relaxed_less_equal(bool(false), bool(false)));




    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), std::string("386")));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(387), std::string("386")));
    EXPECT_FALSE(util::relaxed_less_equal(double(387), std::string("386")));

    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(387), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(387), uint64_t(386)));
    EXPECT_FALSE(util::relaxed_less_equal(double(387), uint64_t(386)));

    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), int64_t(386)));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(387), int64_t(386)));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(387), int64_t(386)));
    EXPECT_FALSE(util::relaxed_less_equal(double(387), int64_t(386)));

    EXPECT_FALSE(util::relaxed_less_equal(std::string("387"), double(386.7)));
    EXPECT_FALSE(util::relaxed_less_equal(uint64_t(387), double(386.7)));
    EXPECT_FALSE(util::relaxed_less_equal(int64_t(387), double(386.7)));
    EXPECT_FALSE(util::relaxed_less_equal(double(387), double(386.7)));
}
