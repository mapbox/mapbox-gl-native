#ifndef MBGL_TEST_UTIL
#define MBGL_TEST_UTIL

#include <mbgl/util/image.hpp>

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
namespace test {

pid_t startServer(const char *executable);
void stopServer(pid_t pid);

uint64_t crc64(const char*, size_t);
uint64_t crc64(const std::string&);

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold = 0,
                double pixelThreshold = 0);

}
}

#endif
