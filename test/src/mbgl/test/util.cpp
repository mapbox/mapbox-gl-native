#include <mbgl/test/util.hpp>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>

#include <mapbox/pixelmatch.hpp>

#include <csignal>
#include <future>

#include <unistd.h>

#ifndef NODE_EXECUTABLE
#define NODE_EXECUTABLE node
#endif

#define xstr(s) str(s)
#define str(s) #s

namespace mbgl {
namespace test {

Server::Server(const char* script) {
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

        const char* executable = xstr(NODE_EXECUTABLE);

        fprintf(stderr, "executable: %s\n", executable);

        // Launch the actual server process.
        int ret = execl(executable, executable, script, nullptr);

        // This call should not return. In case execl failed, we exit anyway.
        if (ret < 0) {
            Log::Error(Event::Setup, "Failed to start server: %s", strerror(errno));
        }
        abort();
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

void checkImage(const std::string& base,
                const PremultipliedImage& actual,
                double imageThreshold,
                double pixelThreshold) {
#if !TEST_READ_ONLY
    if (getenv("UPDATE")) {
        util::write_file(base + "/expected.png", encodePNG(actual));
        return;
    }
#endif

    std::string expected_image;
    try {
        expected_image = util::read_file(base + "/expected.png");
    } catch (std::exception& ex) {
        Log::Error(Event::Setup, "Failed to load expected image %s: %s",
                   (base + "/expected.png").c_str(), ex.what());
        throw;
    }

    PremultipliedImage expected = decodeImage(expected_image);
    PremultipliedImage diff { expected.size };


#if !TEST_READ_ONLY
    util::write_file(base + "/actual.png", encodePNG(actual));
#endif

    ASSERT_EQ(expected.size, actual.size);

    double pixels = mapbox::pixelmatch(actual.data.get(),
                                       expected.data.get(),
                                       expected.size.width,
                                       expected.size.height,
                                       diff.data.get(),
                                       pixelThreshold);

    EXPECT_LE(pixels / (expected.size.width * expected.size.height), imageThreshold);

#if !TEST_READ_ONLY
    util::write_file(base + "/diff.png", encodePNG(diff));
#endif
}

} // namespace test
} // namespace mbgl
