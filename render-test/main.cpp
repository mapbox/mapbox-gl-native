#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/io.hpp>

#include "filesystem.hpp"
#include "metadata.hpp"
#include "parser.hpp"
#include "runner.hpp"

#include <random>
#include <regex>

#define ANSI_COLOR_RED        "\x1b[31m"
#define ANSI_COLOR_GREEN      "\x1b[32m"
#define ANSI_COLOR_YELLOW     "\x1b[33m"
#define ANSI_COLOR_BLUE       "\x1b[34m"
#define ANSI_COLOR_MAGENTA    "\x1b[35m"
#define ANSI_COLOR_CYAN       "\x1b[36m"
#define ANSI_COLOR_GRAY       "\x1b[37m"
#define ANSI_COLOR_LIGHT_GRAY "\x1b[90m"
#define ANSI_COLOR_RESET      "\x1b[0m"

namespace {

TestPaths makeTestPaths(mbgl::filesystem::path stylePath) {
    std::vector<mbgl::filesystem::path> expectations{ stylePath };
    expectations.front().remove_filename();

    const static std::regex regex{ TestRunner::getBasePath() };    
    for (const std::string& path : TestRunner::getPlatformExpectationsPaths()) {
        expectations.emplace_back(std::regex_replace(expectations.front().string(), regex, path));
        assert(!expectations.back().empty());
    }

    return {
        std::move(stylePath),
        std::move(expectations)
    };
}

} // namespace

int main(int argc, char** argv) {
    bool recycleMap;
    bool shuffle;
    uint32_t seed;
    std::string testRootPath;
    std::vector<std::string> ids;

    std::tie(recycleMap, shuffle, seed, testRootPath, ids) = parseArguments(argc, argv);
    const std::string::size_type rootLength = testRootPath.length();

    const auto ignores = parseIgnores();

    // Recursively traverse through the test paths and collect test directories containing "style.json".
    std::vector<TestPaths> testPaths;
    testPaths.reserve(ids.size());
    for (const auto& id : ids) {
        for (auto& testPath : mbgl::filesystem::recursive_directory_iterator(mbgl::filesystem::path(id))) {
            if (testPath.path().filename() == "style.json") {
                testPaths.emplace_back(makeTestPaths(testPath));
            }
        }
    }

    if (shuffle) {
        printf(ANSI_COLOR_YELLOW "Shuffle seed: %d" ANSI_COLOR_RESET "\n", seed);

        std::seed_seq sequence { seed };
        std::mt19937 shuffler(sequence);
        std::shuffle(testPaths.begin(), testPaths.end(), shuffler);
    }

    mbgl::util::RunLoop runLoop;
    TestRunner runner;

    std::vector<TestMetadata> metadatas;
    metadatas.reserve(testPaths.size());

    TestStatistics stats;

    for (auto& testPath : testPaths) {
        TestMetadata metadata = parseTestMetadata(testPath);

        if (!recycleMap) {
            runner.reset();
        }

        std::string& id = metadata.id;
        std::string& status = metadata.status;
        std::string& color = metadata.color;

        id = testPath.defaultExpectations();
        id = id.substr(rootLength + 1, id.length() - rootLength - 2);

        bool shouldIgnore = false;
        std::string ignoreReason;

        const std::string ignoreName = "render-tests/" + id;
        const auto it = std::find_if(ignores.cbegin(), ignores.cend(), [&ignoreName](auto pair) { return pair.first == ignoreName; });
        if (it != ignores.end()) {
            shouldIgnore = true;
            ignoreReason = it->second;
            if (ignoreReason.rfind("skip", 0) == 0) {
                printf(ANSI_COLOR_GRAY "* skipped %s (%s)" ANSI_COLOR_RESET "\n", id.c_str(), ignoreReason.c_str());
                continue;
            }
        }

        bool errored = !metadata.errorMessage.empty();
        if (!errored) {
            errored = runner.run(metadata) && !metadata.errorMessage.empty();
        }

        bool passed = !errored && !metadata.diff.empty() && metadata.difference <= metadata.allowed;

        if (shouldIgnore) {
            if (passed) {
                status = "ignored passed";
                color = "#E8A408";
                stats.ignorePassedTests++;
                printf(ANSI_COLOR_YELLOW "* ignore %s (%s)" ANSI_COLOR_RESET "\n", id.c_str(), ignoreReason.c_str());
            } else {
                status = "ignored failed";
                color = "#9E9E9E";
                stats.ignoreFailedTests++;
                printf(ANSI_COLOR_LIGHT_GRAY "* ignore %s (%s)" ANSI_COLOR_RESET "\n", id.c_str(), ignoreReason.c_str());
            }
        } else {
            if (passed) {
                status = "passed";
                color = "green";
                stats.passedTests++;
                printf(ANSI_COLOR_GREEN "* passed %s" ANSI_COLOR_RESET "\n", id.c_str());
            } else if (errored) {
                status = "errored";
                color = "red";
                stats.erroredTests++;
                printf(ANSI_COLOR_RED "* errored %s" ANSI_COLOR_RESET "\n", id.c_str());
            } else {
                status = "failed";
                color = "red";
                stats.failedTests++;
                printf(ANSI_COLOR_RED "* failed %s" ANSI_COLOR_RESET "\n", id.c_str());
            }
        }

        metadatas.push_back(std::move(metadata));
    }

    std::string resultsHTML = createResultPage(stats, metadatas, shuffle, seed);
    mbgl::util::write_file(testRootPath + "/index.html", resultsHTML);

    const uint32_t count = stats.erroredTests + stats.failedTests +
                           stats.ignoreFailedTests + stats.ignorePassedTests +
                           stats.passedTests;

    if (stats.passedTests) {
        printf(ANSI_COLOR_GREEN "%u passed (%.1lf%%)" ANSI_COLOR_RESET "\n", stats.passedTests, 100.0 * stats.passedTests / count);
    }
    if (stats.ignorePassedTests) {
        printf(ANSI_COLOR_YELLOW "%u passed but were ignored (%.1lf%%)" ANSI_COLOR_RESET "\n", stats.ignorePassedTests, 100.0 * stats.ignorePassedTests / count);
    }
    if (stats.ignoreFailedTests) {
        printf(ANSI_COLOR_LIGHT_GRAY "%u ignored (%.1lf%%)" ANSI_COLOR_RESET "\n", stats.ignoreFailedTests, 100.0 * stats.ignoreFailedTests / count);
    }
    if (stats.failedTests) {
        printf(ANSI_COLOR_RED "%u failed (%.1lf%%)" ANSI_COLOR_RESET "\n", stats.failedTests, 100.0 * stats.failedTests / count);
    }
    if (stats.erroredTests) {
        printf(ANSI_COLOR_RED "%u errored (%.1lf%%)" ANSI_COLOR_RESET "\n", stats.erroredTests, 100.0 * stats.erroredTests / count);
    }

    printf("Results at: %s%s\n", testRootPath.c_str(), "/index.html");

    return stats.failedTests + stats.erroredTests == 0 ? 0 : 1;
}
