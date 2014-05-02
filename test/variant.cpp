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
