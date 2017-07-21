#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if ANDROID
#define TEST_READ_ONLY 0
#define TEST_HAS_SERVER 0
#elif TARGET_OS_IOS
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

#if !TEST_IS_SIMULATOR && !CI_BUILD
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

#if !CI_BUILD
#define TEST_DISABLED_ON_CI(name) name
#else
#define TEST_DISABLED_ON_CI(name) DISABLED_ ## name
#endif

#include <mbgl/util/image.hpp>
#include <mbgl/util/chrono.hpp>

#include <cstdint>
#include <memory>

#include <gtest/gtest.h>

namespace mbgl {
namespace test {

class Server {
public:
    Server(const char* script);
    ~Server();

private:
    int fd = -1;
};

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold = 0,
                double pixelThreshold = 0);

} // namespace test
} // namespace mbgl
