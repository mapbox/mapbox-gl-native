#ifndef MBGL_TEST_UTIL
#define MBGL_TEST_UTIL

#include <gtest/gtest.h>

#define SCOPED_TEST(name) \
    static class name { \
        bool completed = false; \
    public: \
        void finish() { EXPECT_FALSE(completed) << #name " was already completed."; completed = true; } \
        ~name() { if (!completed) ADD_FAILURE() << #name " didn't complete."; } \
    } name;

#endif
