#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IOS
#define TEST_READ_ONLY 1
#define TEST_HAS_SERVER 0
#else
#define TEST_READ_ONLY 0
#define TEST_HAS_SERVER 1
#endif

#if TARGET_OS_SIMULATOR
#define TEST_IS_SIMULATOR 1
#else
#define TEST_IS_SIMULATOR 0
#endif

#if !TEST_IS_SIMULATOR
#define TEST_REQUIRES_ACCURATE_TIMING(name) name
#else
#define TEST_REQUIRES_ACCURATE_TIMING(name) DISABLED_ ## name
#endif

#if !TEST_READ_ONLY
#define TEST_REQUIRES_WRITE(name) name
#else
#define TEST_REQUIRES_WRITE(name) DISABLED_ ## name
#endif

#if TEST_HAS_SERVER
#define TEST_REQUIRES_SERVER(name) name
#else
#define TEST_REQUIRES_SERVER(name) DISABLED_ ## name
#endif

#include <mbgl/util/image.hpp>
#include <mbgl/util/chrono.hpp>

#include <cstdint>

#include <gtest/gtest.h>

#define SCOPED_TEST(name) \
    static class name { \
        bool completed = false; \
    public: \
        void finish() { EXPECT_FALSE(completed) << #name " was already completed."; completed = true; } \
        ~name() { if (!completed) ADD_FAILURE() << #name " didn't complete."; } \
    } name;

namespace mbgl {

class Map;

namespace test {

class Server {
public:
    Server(const char* script);
    ~Server();

private:
    int fd = -1;
};

PremultipliedImage render(Map&);

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold = 0,
                double pixelThreshold = 0);

} // namespace test
} // namespace mbgl
