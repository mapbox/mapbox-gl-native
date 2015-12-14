#include "util.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

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

std::string getFileSourceRoot() {
#ifdef MBGL_ASSET_ZIP
    return "test/fixtures/annotations/assets.zip";
#else
    return "";
#endif
}

pid_t startServer(const char *executable) {
    int pipefd[2];

    if (pipe(pipefd)) {
        throw std::runtime_error("Cannot create server pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        Log::Error(Event::Setup, "Cannot create server process");
        exit(1);
    } else if (pid == 0) {
        // This is the child process.

        // Close the input side of the pipe.
        close(pipefd[0]);

        // Launch the actual server process, with the pipe ID as the first argument.
        char *args[] = { const_cast<char *>(executable),
                         const_cast<char *>(std::to_string(pipefd[1]).c_str()),
                         nullptr };
        int ret = execv(executable, args);
        // This call should not return. In case execve failed, we exit anyway.
        if (ret < 0) {
            Log::Error(Event::Setup, "Failed to start server: %s", strerror(errno));
        }
        exit(0);
    } else {
        // This is the parent process.

        // Close output side of the pipe.
        close(pipefd[1]);

        // Wait until the server process closes the handle.
        char buffer[2];
        ssize_t bytes = 0, total = 0;
        while ((bytes = read(pipefd[0], buffer, sizeof(buffer))) != 0) {
            total += bytes;
        }
        if (bytes < 0) {
            Log::Error(Event::Setup, "Failed to start server: %s", strerror(errno));
            exit(1);
        }
        if (total != 2 || strncmp(buffer, "OK", 2) != 0) {
            Log::Error(Event::Setup, "Failed to start server");
            exit(1);
        }
        close(pipefd[0]);
    }
    return pid;
}

void stopServer(pid_t pid) {
    kill(pid, SIGTERM);
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

PremultipliedImage render(Map& map) {
    std::promise<PremultipliedImage> promise;
    map.renderStill([&](std::exception_ptr, PremultipliedImage&& image) {
        promise.set_value(std::move(image));
    });
    return promise.get_future().get();
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
