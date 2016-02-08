#include "util.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/chrono.hpp>

#include <mapbox/pixelmatch.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/crc.hpp>
#pragma GCC diagnostic pop

#include <csignal>
#include <future>

#include <unistd.h>

namespace mbgl {
namespace test {

Server::Server(const char* executable) {
    int input[2];
    int output[2];

    if (pipe(input)) {
        throw std::runtime_error("Cannot create server input pipe");
    }
    if (pipe(output)) {
        throw std::runtime_error("Cannot create server output pipe");
    }

    // Store the parent => child pipe so that we can close it in the destructor.
    fd = input[1];

    pid_t pid = fork();
    if (pid < 0) {
        Log::Error(Event::Setup, "Cannot create server process");
        exit(1);
    } else if (pid == 0) {
        // This is the child process.

        // Connect the parent => child pipe to stdin.
        while ((dup2(input[0], STDIN_FILENO) == -1) && (errno == EINTR)) {}
        close(input[0]);
        close(input[1]);

        // Move the child => parent side of the pipe to stdout.
        while ((dup2(output[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
        close(output[1]);
        close(output[0]);

        // Launch the actual server process.
        int ret = execl(executable, executable, nullptr);

        // This call should not return. In case execl failed, we exit anyway.
        if (ret < 0) {
            Log::Error(Event::Setup, "Failed to start server: %s", strerror(errno));
        }
        exit(0);
    } else {
        // This is the parent process.

        // Close the unneeded sides of the pipes.
        close(output[1]);
        close(input[0]);

        // Wait until the server process sends at least 2 bytes or closes the handle.
        char buffer[2];
        ssize_t bytes, total = 0;
        while (total < 2 && (bytes = read(output[0], buffer + total, 2 - total)) != 0) {
            total += bytes;
        }

        // Close child => parent pipe.
        close(output[0]);

        // Check signature
        if (total != 2 || strncmp(buffer, "OK", 2) != 0) {
            throw std::runtime_error("Failed to start server: Invalid signature");
        }
    }
}

Server::~Server() {
    if (fd > 0) {
        close(fd);
    }
}


// from https://gist.github.com/ArtemGr/997887
uint64_t crc64(const char* data, size_t size) {
    boost::crc_optimal<64, 0x04C11DB7, 0, 0, false, false> crc;
    crc.process_bytes(data, size);
    return crc.checksum();
}

uint64_t crc64(const std::string& str) {
    return crc64(str.data(), str.size());
}

uint64_t crc64(const PremultipliedImage &image) {
    return crc64(reinterpret_cast<const char*>(image.data.get()), image.size());
}

PremultipliedImage render(Map& map, Milliseconds timeout) {
    std::promise<PremultipliedImage> promise;
    map.renderStill([&](std::exception_ptr, PremultipliedImage&& image) {
        promise.set_value(std::move(image));
    });

    // Limit maximum wait time.
    auto future = promise.get_future();
    if (future.wait_for(timeout) != std::future_status::ready) {
        // Alas, we didn't get the promised future :(
        Log::Error(Event::Image, "Failed to generate image within %dms", timeout.count());
        return { 0, 0 };
    }
    return future.get();
}

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold,
                double pixelThreshold) {
    if (getenv("UPDATE")) {
        util::write_file(base + "/expected.png", encodePNG(actual));
        return;
    }

    PremultipliedImage expected = decodeImage(util::read_file(base + "/expected.png"));
    PremultipliedImage diff { expected.width, expected.height };

    ASSERT_EQ(expected.width, actual.width);
    ASSERT_EQ(expected.height, actual.height);

    double pixels = mapbox::pixelmatch(actual.data.get(),
                                       expected.data.get(),
                                       expected.width,
                                       expected.height,
                                       diff.data.get(),
                                       pixelThreshold);

    EXPECT_LE(pixels / (expected.width * expected.height), imageThreshold);

    util::write_file(base + "/actual.png", encodePNG(actual));
    util::write_file(base + "/diff.png", encodePNG(diff));
}

} // namespace test
} // namespace mbgl
