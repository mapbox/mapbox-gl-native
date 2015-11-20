#include "util.hpp"

#include <mbgl/platform/log.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

#include <mapbox/pixelmatch.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <boost/crc.hpp>
#pragma GCC diagnostic pop

#include <csignal>

#include <unistd.h>

namespace mbgl {
namespace test {

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

UnassociatedImage unpremultiply(const PremultipliedImage& src) {
    UnassociatedImage dst { src.width, src.height };
    std::copy(src.data.get(), src.data.get() + src.size(), dst.data.get());

    uint8_t* data = dst.data.get();
    for (size_t i = 0; i < dst.size(); i += 4) {
        uint8_t& r = data[i + 0];
        uint8_t& g = data[i + 1];
        uint8_t& b = data[i + 2];
        uint8_t& a = data[i + 3];
        if (a) {
            r = (255 * r + (a / 2)) / a;
            g = (255 * g + (a / 2)) / a;
            b = (255 * b + (a / 2)) / a;
        }
    }

    return std::move(dst);
}

void checkImage(const std::string& base,
                const UnassociatedImage& actual,
                double imageThreshold,
                double pixelThreshold) {
    // TODO: the pixels produced by Map::renderStill are probably actually premultiplied,
    // but Map::renderStill produces an UnassociatedImage. This probably should be fixed;
    // here we just hack around it by copying the pixels to a PremultipliedImage (and
    // un-premultiplying them when updating expected.png, since encodePNG wants
    // un-premultiplied pixels).
    PremultipliedImage actualActual { actual.width, actual.height };
    std::copy(actual.data.get(), actual.data.get() + actual.size(), actualActual.data.get());

    if (getenv("UPDATE")) {
        util::write_file(base + "/expected.png", encodePNG(unpremultiply(actualActual)));
        return;
    }

    PremultipliedImage expected = decodeImage(util::read_file(base + "/expected.png"));
    UnassociatedImage diff { expected.width, expected.height };

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

}
}
