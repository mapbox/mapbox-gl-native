#ifndef MBGL_TEST_UTIL
#define MBGL_TEST_UTIL

#include <mbgl/util/image.hpp>

#include <chrono>
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
    Server(const char* executable);
    ~Server();

private:
    int fd = -1;
};

uint64_t crc64(const char*, size_t);
uint64_t crc64(const std::string&);
uint64_t crc64(const PremultipliedImage&);

PremultipliedImage render(Map&,
                          std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold = 0,
                double pixelThreshold = 0);
}
}

#endif
