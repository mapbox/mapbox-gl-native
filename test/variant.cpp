#include <iostream>
#include "gtest/gtest.h"

#include <llmr/style/value.hpp>
#include <boost/lexical_cast.hpp>

using namespace llmr;

TEST(Variant, toString) {
    EXPECT_EQ("lord", llmr::toString( (std::string)"lord" ));
    EXPECT_EQ("36", llmr::toString( (uint64_t)36 ));
    EXPECT_EQ("-239", llmr::toString( (int64_t)-239 ));
    EXPECT_EQ("0", llmr::toString( (int64_t)0 ));
    EXPECT_EQ("3.14159268", llmr::toString( (double)3.14159268 ));
    EXPECT_EQ("3.1415926535897931", llmr::toString( (double)3.1415926535897931 ));
    EXPECT_EQ("true", llmr::toString( true ));
    EXPECT_EQ("false", llmr::toString( false ));
}


TEST(Variant, RelaxedEquality) {
    // Compare to bool
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(true))(int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(true))(int64_t(-7042)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(int64_t(0)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(true))(uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(uint64_t(0)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(true))(double(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(double(0)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(true))(std::string("386")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("\t")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("  ")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("false")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string(" false")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("False")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("null")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("nuLL")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string("NULL")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(std::string(" NULL  ")));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(true))(bool(true)));
    EXPECT_TRUE(util::relaxed_equal<Value>(bool(false))(bool(false)));

    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(int64_t(0)));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(uint64_t(0)));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(double(0)));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(std::string("")));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(std::string("false")));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(std::string("FALSE")));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(std::string("null")));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(std::string("Null")));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(false))(int64_t(-754)));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(false))(uint64_t(3)));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(false))(double(3.145)));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(false))(std::string("test")));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(true))(bool(false)));
    EXPECT_FALSE(util::relaxed_equal<Value>(bool(false))(bool(true)));


    // Compare to int64_t
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(386))(int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(-7042))(int64_t(-7042)));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(386))(uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(386))(double(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(386))(std::string("386")));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(1))(bool(true)));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(0))(bool(false)));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(1))(std::string("true")));
    EXPECT_TRUE(util::relaxed_equal<Value>(int64_t(0))(std::string("false")));

    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(386))(int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(-7042))(int64_t(-7043)));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(386))(uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(386))(double(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(386))(std::string("387")));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(1))(bool(false)));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(0))(bool(true)));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(1))(std::string("false")));
    EXPECT_FALSE(util::relaxed_equal<Value>(int64_t(0))(std::string("true")));


    // Compare to uint64_t
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(386))(int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(386))(uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(0))(uint64_t(0)));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(386))(double(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(386))(std::string("386")));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(1))(bool(true)));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(0))(bool(false)));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(1))(std::string("true")));
    EXPECT_TRUE(util::relaxed_equal<Value>(uint64_t(0))(std::string("false")));

    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(386))(int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(386))(uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(0))(uint64_t(1)));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(386))(double(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(386))(std::string("387")));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(1))(bool(false)));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(0))(bool(true)));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(1))(std::string("false")));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(0))(std::string("true")));
    EXPECT_FALSE(util::relaxed_equal<Value>(uint64_t(0))(std::string(" true")));


    // Compare to double
    EXPECT_TRUE(util::relaxed_equal<Value>(double(3.159385))(double(3.159385)));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(386))(int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(386))(uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(0))(uint64_t(0)));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(386))(double(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(386))(std::string("386")));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(1))(bool(true)));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(0))(bool(false)));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(1))(std::string("true")));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(1))(std::string("TRue")));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(0))(std::string("false")));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(0))(std::string("FALSe")));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(0))(std::string("null")));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(0))(std::string("Null")));
    EXPECT_TRUE(util::relaxed_equal<Value>(double(0))(std::string("  Null ")));

    EXPECT_FALSE(util::relaxed_equal<Value>(double(3.159385))(double(3.159383)));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(386))(int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(386))(uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(0))(uint64_t(1)));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(386))(double(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(386))(std::string("387")));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(1))(bool(false)));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(0))(bool(true)));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(1))(std::string("false")));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(1))(std::string("False")));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(1))(std::string("null")));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(1))(std::string("NULL")));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(0))(std::string("true")));
    EXPECT_FALSE(util::relaxed_equal<Value>(double(0))(std::string("TRUE")));


    // Compare to string
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("lord"))(std::string("lord")));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("386"))(int64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("-386"))(int64_t(-386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("386"))(uint64_t(386)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("386.36"))(double(386.36)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("true"))(bool(true)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("True"))(bool(true)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("some string"))(bool(true)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("false"))(bool(false)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("fAlse"))(bool(false)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("null"))(bool(false)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string("NULl"))(bool(false)));
    EXPECT_TRUE(util::relaxed_equal<Value>(std::string(""))(bool(false)));

    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("lord"))(std::string("baron")));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("386"))(int64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("-386"))(int64_t(-387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("386"))(uint64_t(387)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("386.36"))(double(386.37)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("true"))(bool(false)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("TRUE"))(bool(false)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("some string"))(bool(false)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("false"))(bool(true)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("FALSE"))(bool(true)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("null"))(bool(true)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string("NULl"))(bool(true)));
    EXPECT_FALSE(util::relaxed_equal<Value>(std::string(""))(bool(true)));
}
