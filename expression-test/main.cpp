#include "expression_test_parser.hpp"
#include "expression_test_runner.hpp"
#include "expression_test_logger.hpp"

#include <random>

int main(int argc, char** argv) {
    // Parse args
    std::vector<mbgl::filesystem::path> testPaths;
    mbgl::filesystem::path rootPath;
    bool shuffle;
    uint32_t seed;
    std::tie(rootPath, testPaths, shuffle, seed) = parseArguments(argc, argv);

    // Parse ignores
    const auto ignores = parseExpressionIgnores();

    if (shuffle) {
        printf(ANSI_COLOR_YELLOW "Shuffle seed: %d" ANSI_COLOR_RESET "\n", seed);
        std::seed_seq sequence { seed };
        std::mt19937 shuffler(sequence);
        std::shuffle(testPaths.begin(), testPaths.end(), shuffler);
    }

    // Run tests, collect stats and output of each run.
    TestStats stats;
    for (const auto& path : testPaths) {
        const auto& expectation = path.parent_path().string();
        std::string id = expectation.substr(rootPath.string().length() + 1, expectation.length() - rootPath.string().length());
        stats.ids.emplace_back(id);

        bool shouldIgnore = false;
        std::string ignoreReason;
        const std::string ignoreName = "expression-tests/" + id;
        const auto it = std::find_if(ignores.cbegin(), ignores.cend(), [&ignoreName] (const auto& ignore) { return ignore.id == ignoreName; });
        if (it != ignores.end()) {
            shouldIgnore = true;
            ignoreReason = (*it).reason;
        }

        optional<TestRunOutput> testRun;
        if (auto testData = parseTestData(path)) {
            testRun = runExpressionTest(*testData, rootPath.string(), id);
        }

        if (!testRun) {
            stats.errored.emplace_back(id);
            printf(ANSI_COLOR_RED "* ERROR can't parse '%s' test" ANSI_COLOR_RESET "\n", id.c_str());
            continue;
        }

        if (shouldIgnore) {
            if (testRun->passed) {
                stats.ignorePassed.emplace_back(std::move(*testRun));
                printf(ANSI_COLOR_YELLOW "* PASSED ignored test %s (%s)" ANSI_COLOR_RESET "\n", id.c_str(), ignoreReason.c_str());
            } else {
                stats.ignoreFailed.emplace_back(std::move(*testRun));
                printf(ANSI_COLOR_LIGHT_GRAY "* FAILED ignored test %s (%s)" ANSI_COLOR_RESET "\n", id.c_str(), ignoreReason.c_str());
            }
        } else {
            if (testRun->passed) {
                stats.passed.emplace_back(std::move(*testRun));
                printf(ANSI_COLOR_GREEN "* PASSED %s" ANSI_COLOR_RESET "\n", id.c_str());
            } else {
                printf(ANSI_COLOR_RED "* FAILED %s\n%s" ANSI_COLOR_RESET, id.c_str(), testRun->text.c_str());
                stats.failed.emplace_back(std::move(*testRun));
            }
        }
    }

    printStats(stats);
    writeHTMLResults(stats, rootPath.string(), shuffle, seed);

    return stats.errored.size() + stats.failed.size() == 0 ? 0 : 1;
}
