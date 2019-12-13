#include <mbgl/map/camera.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/renderer/renderer_observer.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/light.hpp>
#include <mbgl/style/rapidjson_conversion.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/string.hpp>

#include <mapbox/pixelmatch.hpp>

#include <../expression-test/test_runner_common.hpp>
#include "allocation_index.hpp"
#include "file_source.hpp"
#include "metadata.hpp"
#include "parser.hpp"
#include "runner.hpp"

#include <algorithm>
#include <cassert>
#include <utility>
#include <sstream>

using namespace mbgl;
using namespace TestOperationNames;

GfxProbe::GfxProbe(const mbgl::gfx::RenderingStats& stats, const GfxProbe& prev)
    : numBuffers(stats.numBuffers),
      numDrawCalls(stats.numDrawCalls),
      numFrameBuffers(stats.numFrameBuffers),
      numTextures(stats.numActiveTextures),
      memIndexBuffers(stats.memIndexBuffers, std::max(stats.memIndexBuffers, prev.memIndexBuffers.peak)),
      memVertexBuffers(stats.memVertexBuffers, std::max(stats.memVertexBuffers, prev.memVertexBuffers.peak)),
      memTextures(stats.memTextures, std::max(stats.memTextures, prev.memTextures.peak)) {}

// static
gfx::HeadlessBackend::SwapBehaviour swapBehavior(MapMode mode) {
    return mode == MapMode::Continuous ? gfx::HeadlessBackend::SwapBehaviour::Flush
                                       : gfx::HeadlessBackend::SwapBehaviour::NoFlush;
}

std::string simpleDiff(const Value& result, const Value& expected) {
    std::vector<std::string> resultTokens{tokenize(toJSON(result, 2, false))};
    std::vector<std::string> expectedTokens{tokenize(toJSON(expected, 2, false))};
    std::size_t maxLength = std::max(resultTokens.size(), expectedTokens.size());
    std::ostringstream diff;

    diff << "<pre>" << std::endl;
    const auto flush =
        [](const std::vector<std::string>& vec, std::size_t pos, std::ostringstream& out, std::string separator) {
            for (std::size_t j = pos; j < vec.size(); ++j) {
                out << separator << vec[j] << std::endl;
            }
        };

    for (std::size_t i = 0; i < maxLength; ++i) {
        if (resultTokens.size() <= i) {
            flush(expectedTokens, i, diff, "-");
            break;
        }

        if (expectedTokens.size() <= i) {
            flush(resultTokens, i, diff, "+");
            break;
        }

        if (!deepEqual(resultTokens[i], expectedTokens[i])) {
            diff << "<b>"
                 << "-" << expectedTokens[i] << "</b>" << std::endl;
            diff << "<b>"
                 << "+" << resultTokens[i] << "</b>" << std::endl;
        } else {
            diff << resultTokens[i] << std::endl;
        }
    }
    diff << "</pre>" << std::endl;
    return diff.str();
}

TestRunner::TestRunner(Manifest manifest_, UpdateResults updateResults_)
    : manifest(std::move(manifest_)), updateResults(updateResults_) {}

const Manifest& TestRunner::getManifest() const {
    return manifest;
}

void TestRunner::doShuffle(uint32_t seed) {
    manifest.doShuffle(seed);
}

bool TestRunner::checkQueryTestResults(mbgl::PremultipliedImage&& actualImage,
                                       std::vector<mbgl::Feature>&& features,
                                       TestMetadata& metadata) {
    const std::string& base = metadata.paths.defaultExpectations();
    const std::vector<mbgl::filesystem::path>& expectations = metadata.paths.expectations;

    metadata.actual = mbgl::encodePNG(actualImage);

    if (actualImage.size.isEmpty()) {
        metadata.errorMessage = "Invalid size for actual image";
        return false;
    }

    metadata.actualJson = toJSON(features, 2, false);

    if (metadata.actualJson.empty()) {
        metadata.errorMessage = "Invalid size for actual JSON";
        return false;
    }

    if (updateResults == UpdateResults::PLATFORM) {
        mbgl::filesystem::create_directories(expectations.back());
        mbgl::util::write_file(expectations.back().string() + "/expected.json", metadata.actualJson);
        return true;
    } else if (updateResults == UpdateResults::DEFAULT) {
        mbgl::util::write_file(base + "/expected.json", metadata.actualJson);
        return true;
    }

    mbgl::util::write_file(base + "/actual.json", metadata.actualJson);

    std::vector<std::string> expectedJsonPaths;
    mbgl::filesystem::path expectedMetricsPath;
    for (auto rit = expectations.rbegin(); rit != expectations.rend(); ++rit) {
        if (mbgl::filesystem::exists(*rit)) {
            expectedJsonPaths = readExpectedJSONEntries(*rit);
            if (!expectedJsonPaths.empty()) break;
        }
    }

    if (expectedJsonPaths.empty()) {
        metadata.errorMessage = "Failed to find expectations for: " + metadata.paths.stylePath.string();
        return false;
    }

    for (const auto& entry : expectedJsonPaths) {
        auto maybeExpectedJson = readJson(entry);
        if (maybeExpectedJson.is<mbgl::JSDocument>()) {
            auto& expected = maybeExpectedJson.get<mbgl::JSDocument>();

            mbgl::JSDocument actual;
            actual.Parse<0>(metadata.actualJson);
            if (actual.HasParseError()) {
                metadata.errorMessage = "Error parsing actual JSON for: " + metadata.paths.stylePath.string();
                return false;
            }

            auto actualVal = mapbox::geojson::convert<mapbox::geojson::value>(actual);
            auto expectedVal = mapbox::geojson::convert<mapbox::geojson::value>(expected);
            bool equal = deepEqual(actualVal, expectedVal);

            metadata.difference = !equal;
            if (equal) {
                metadata.diff = "Match";
            } else {
                metadata.diff = simpleDiff(actualVal, expectedVal);
            }
        } else {
            metadata.errorMessage = "Failed to load expected JSON " + entry;
            return false;
        }
    }

    return true;
}

bool TestRunner::checkRenderTestResults(mbgl::PremultipliedImage&& actualImage, TestMetadata& metadata) {
    const std::string& base = metadata.paths.defaultExpectations();
    const std::vector<mbgl::filesystem::path>& expectations = metadata.paths.expectations;

    if (metadata.outputsImage) {
        metadata.actual = mbgl::encodePNG(actualImage);

        if (actualImage.size.isEmpty()) {
            metadata.errorMessage = "Invalid size for actual image";
            return false;
        }

        if (updateResults == UpdateResults::PLATFORM) {
            mbgl::filesystem::create_directories(expectations.back());
            mbgl::util::write_file(expectations.back().string() + "/expected.png", mbgl::encodePNG(actualImage));
            return true;
        } else if (updateResults == UpdateResults::DEFAULT) {
            mbgl::util::write_file(base + "/expected.png", mbgl::encodePNG(actualImage));
            return true;
        }

        mbgl::util::write_file(base + "/actual.png", metadata.actual);

        mbgl::PremultipliedImage expectedImage{actualImage.size};
        mbgl::PremultipliedImage imageDiff{actualImage.size};

        double pixels = 0.0;
        std::vector<std::string> expectedImagesPaths;
        for (auto rit = expectations.rbegin(); rit != expectations.rend(); ++rit) {
            if (mbgl::filesystem::exists(*rit)) {
                expectedImagesPaths = readExpectedImageEntries(*rit);
                if (!expectedImagesPaths.empty()) break;
            }
        }

        if (expectedImagesPaths.empty()) {
            metadata.errorMessage = "Failed to find expectations for: " + metadata.paths.stylePath.string();
            return false;
        }

        for (const auto& entry : expectedImagesPaths) {
            mbgl::optional<std::string> maybeExpectedImage = mbgl::util::readFile(entry);
            if (!maybeExpectedImage) {
                metadata.errorMessage = "Failed to load expected image " + entry;
                return false;
            }

            metadata.expected = *maybeExpectedImage;

            expectedImage = mbgl::decodeImage(*maybeExpectedImage);

            pixels = // implicitly converting from uint64_t
                mapbox::pixelmatch(actualImage.data.get(),
                                   expectedImage.data.get(),
                                   expectedImage.size.width,
                                   expectedImage.size.height,
                                   imageDiff.data.get(),
                                   0.1285); // Defined in GL JS

            metadata.diff = mbgl::encodePNG(imageDiff);

            mbgl::util::write_file(base + "/diff.png", metadata.diff);

            metadata.difference = pixels / expectedImage.size.area();
            if (metadata.difference <= metadata.allowed) {
                break;
            }
        }
    }
    return true;
}

bool TestRunner::checkProbingResults(TestMetadata& resultMetadata) {
    if (resultMetadata.metrics.isEmpty() || resultMetadata.ignoredTest) return true;
    const auto writeMetrics = [&resultMetadata](const mbgl::filesystem::path& path,
                                                const std::string& message = std::string()) {
        mbgl::filesystem::create_directories(path);
        mbgl::util::write_file(path / "metrics.json", serializeMetrics(resultMetadata.metrics));
        resultMetadata.errorMessage += message;
    };

    const std::vector<mbgl::filesystem::path>& expectedMetrics = resultMetadata.paths.expectedMetrics;
    if (updateResults == UpdateResults::METRICS) {
        writeMetrics(expectedMetrics.back(), " Updated expected metrics.");
        return false;
    }

    // Check the possible paths in reverse order, so that the default path with the test style will only be checked in
    // the very end.
    std::vector<std::string> expectedMetricsPaths;
    for (auto rit = expectedMetrics.rbegin(); rit != expectedMetrics.rend(); ++rit) {
        if (mbgl::filesystem::exists(*rit)) {
            expectedMetricsPaths = readExpectedMetricEntries(*rit);
            if (!expectedMetricsPaths.empty()) break;
        }
    }

    // In case no metrics.json is found, skip assigning the expectedMetrics to metadata, otherwise, take the first found
    // metrics.
    for (const auto& entry : expectedMetricsPaths) {
        auto maybeExpectedMetrics = readExpectedMetrics(entry);
        if (maybeExpectedMetrics.isEmpty()) {
            resultMetadata.errorMessage = "Failed to load expected metrics " + entry;
            return false;
        }
        resultMetadata.expectedMetrics = maybeExpectedMetrics;
        break;
    }

    if (resultMetadata.expectedMetrics.isEmpty()) {
        resultMetadata.errorMessage =
            "Failed to find metric expectations for: " + resultMetadata.paths.stylePath.string();
        if (updateResults == UpdateResults::REBASELINE) {
            writeMetrics(expectedMetrics.back(), ". Created baseline for missing metrics.");
        }
        return false;
    }

    // Check file size metrics.
    auto checkFileSize = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.fileSize.empty()) return true;
        bool passed = true;
        for (const auto& expected : metadata.expectedMetrics.fileSize) {
            auto actual = metadata.metrics.fileSize.find(expected.first);
            if (actual == metadata.metrics.fileSize.end()) {
                metadata.errorMessage = "Failed to find fileSize probe: " + expected.first;
                return false;
            }
            if (actual->second.path != expected.second.path) {
                std::stringstream ss;
                ss << "Comparing different files at probe \"" << expected.first << "\": " << actual->second.path
                   << ", expected is " << expected.second.path << ".";
                metadata.errorMessage = ss.str();

                return false;
            }

            auto result = checkValue(expected.second.size, actual->second.size, actual->second.tolerance);
            if (!std::get<bool>(result)) {
                std::stringstream ss;
                ss << "File size does not match at probe \"" << expected.first << "\" for file \""
                   << expected.second.path << "\": " << actual->second.size << ", expected is " << expected.second.size
                   << ".";

                metadata.errorMessage += metadata.errorMessage.empty() ? ss.str() : "\n" + ss.str();
                passed = false;
                continue;
            }
        }
        return passed;
    };
    auto checkMemory = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.memory.empty()) return true;
#if !defined(SANITIZE)
        // Check memory metrics.
        for (const auto& expected : metadata.expectedMetrics.memory) {
            auto actual = metadata.metrics.memory.find(expected.first);
            if (actual == metadata.metrics.memory.end()) {
                metadata.errorMessage = "Failed to find memory probe: " + expected.first;
                return false;
            }
            bool passed{false};
            float delta{0.0f};
            std::stringstream errorStream;
            std::tie(passed, delta) = MemoryProbe::checkPeak(expected.second, actual->second);
            if (!passed) {
                errorStream << "Allocated memory peak size at probe \"" << expected.first << "\" is "
                            << actual->second.peak << " bytes, expected is " << expected.second.peak << "±" << delta
                            << " bytes.";
            }

            std::tie(passed, delta) = MemoryProbe::checkAllocations(expected.second, actual->second);
            if (!passed) {
                errorStream << "Number of allocations at probe \"" << expected.first << "\" is "
                            << actual->second.allocations << ", expected is " << expected.second.allocations << "±"
                            << std::round(delta) << " allocations.";
            }

            metadata.errorMessage = errorStream.str();
            if (!metadata.errorMessage.empty()) return false;
        }

#endif // !defined(SANITIZE)
        return true;
    };

    // Check network metrics.
    auto checkNetwork = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.network.empty()) return true;
#if !defined(SANITIZE)
        for (const auto& expected : metadata.expectedMetrics.network) {
            auto actual = metadata.metrics.network.find(expected.first);
            if (actual == metadata.metrics.network.end()) {
                metadata.errorMessage = "Failed to find network probe: " + expected.first;
                return false;
            }
            bool failed = false;
            if (actual->second.requests != expected.second.requests) {
                std::stringstream ss;
                ss << "Number of requests at probe \"" << expected.first << "\" is " << actual->second.requests
                   << ", expected is " << expected.second.requests << ". ";

                metadata.errorMessage = ss.str();
                failed = true;
            }
            if (actual->second.transferred != expected.second.transferred) {
                std::stringstream ss;
                ss << "Transferred data at probe \"" << expected.first << "\" is " << actual->second.transferred
                   << " bytes, expected is " << expected.second.transferred << " bytes.";

                metadata.errorMessage += ss.str();
                failed = true;
            }
            if (failed) {
                return false;
            }
        }
#endif // !defined(SANITIZE)
        return true;
    };
    // Check fps metrics
    auto checkFps = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.fps.empty()) return true;
        for (const auto& expected : metadata.expectedMetrics.fps) {
            auto actual = metadata.metrics.fps.find(expected.first);
            if (actual == metadata.metrics.fps.end()) {
                metadata.errorMessage = "Failed to find fps probe: " + expected.first;
                return false;
            }
            auto result = checkValue(expected.second.average, actual->second.average, expected.second.tolerance);
            if (!std::get<bool>(result)) {
                std::stringstream ss;
                ss << "Average fps at probe \"" << expected.first << "\" is " << actual->second.average
                   << ", expected to be " << expected.second.average << " with tolerance of "
                   << expected.second.tolerance;
                metadata.errorMessage = ss.str();
                return false;
            }
            result = checkValue(expected.second.minOnePc, actual->second.minOnePc, expected.second.tolerance);
            if (!std::get<bool>(result)) {
                std::stringstream ss;
                ss << "Minimum(1%) fps at probe \"" << expected.first << "\" is " << actual->second.minOnePc
                   << ", expected to be " << expected.second.minOnePc << " with tolerance of "
                   << expected.second.tolerance;
                metadata.errorMessage = ss.str();
                return false;
            }
        }
        return true;
    };
    // Check gfx metrics
    auto checkGfx = [](TestMetadata& metadata) -> bool {
        if (metadata.metrics.gfx.empty()) return true;
        for (const auto& expected : metadata.expectedMetrics.gfx) {
            auto actual = metadata.metrics.gfx.find(expected.first);
            if (actual == metadata.metrics.gfx.end()) {
                metadata.errorMessage = "Failed to find gfx probe: " + expected.first;
                return false;
            }

            const auto& probeName = expected.first;
            const auto& expectedValue = expected.second;
            const auto& actualValue = actual->second;
            bool failed = false;

            if (expectedValue.numDrawCalls != actualValue.numDrawCalls) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of draw calls at probe\"" << probeName << "\" is " << actualValue.numDrawCalls
                   << ", expected is " << expectedValue.numDrawCalls;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.numTextures != actualValue.numTextures) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of textures at probe \"" << probeName << "\" is " << actualValue.numTextures
                   << ", expected is " << expectedValue.numTextures;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.numBuffers != actualValue.numBuffers) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of vertex and index buffers at probe \"" << probeName << "\" is "
                   << actualValue.numBuffers << ", expected is " << expectedValue.numBuffers;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.numFrameBuffers != actualValue.numFrameBuffers) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Number of frame buffers at probe \"" << probeName << "\" is " << actualValue.numFrameBuffers
                   << ", expected is " << expectedValue.numFrameBuffers;
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.memTextures.peak != actualValue.memTextures.peak) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Allocated texture memory peak size at probe \"" << probeName << "\" is "
                   << actualValue.memTextures.peak << " bytes, expected is " << expectedValue.memTextures.peak
                   << " bytes";
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.memIndexBuffers.peak != actualValue.memIndexBuffers.peak) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Allocated index buffer memory peak size at probe \"" << probeName << "\" is "
                   << actualValue.memIndexBuffers.peak << " bytes, expected is " << expectedValue.memIndexBuffers.peak
                   << " bytes";
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (expectedValue.memVertexBuffers.peak != actualValue.memVertexBuffers.peak) {
                std::stringstream ss;
                if (!metadata.errorMessage.empty()) ss << std::endl;
                ss << "Allocated vertex buffer memory peak size at probe \"" << probeName << "\" is "
                   << actualValue.memVertexBuffers.peak << " bytes, expected is " << expectedValue.memVertexBuffers.peak
                   << " bytes";
                metadata.errorMessage += ss.str();
                failed = true;
            }

            if (failed) return false;
        }
        return true;
    };

    bool checkResult = checkFileSize(resultMetadata) && checkMemory(resultMetadata) && checkNetwork(resultMetadata) &&
                       checkFps(resultMetadata) && checkGfx(resultMetadata);

    if (!checkResult && updateResults == UpdateResults::REBASELINE) {
        writeMetrics(expectedMetrics.back(), " Rebaselined expected metric for failed test.");
    }

    return checkResult;
}

namespace {

TestOperation unsupportedOperation(const std::string& operation) {
    return [operation](TestContext& ctx) {
        ctx.getMetadata().errorMessage = std::string("Unsupported operation: ") + operation;
        return false;
    };
}

TestOperations getBeforeOperations(const Manifest& manifest) {
    static const std::string mark = " - default - start";
    TestOperations result;
    for (const std::string& probe : manifest.getProbes()) {
        if (memoryProbeOp == probe) {
            result.emplace_back([](TestContext& ctx) {
                assert(!AllocationIndex::isActive());
                AllocationIndex::setActive(true);
                ctx.getMetadata().metrics.memory.emplace(std::piecewise_construct,
                                                         std::forward_as_tuple(memoryProbeOp + mark),
                                                         std::forward_as_tuple(AllocationIndex::getAllocatedSizePeak(),
                                                                               AllocationIndex::getAllocationsCount()));
                return true;
            });
            continue;
        }
        if (gfxProbeOp == probe) {
            result.emplace_back([](TestContext& ctx) {
                assert(!ctx.gfxProbeActive);
                ctx.gfxProbeActive = true;
                ctx.baselineGfxProbe = ctx.activeGfxProbe;
                return true;
            });
            continue;
        }

        if (networkProbeOp == probe) {
            result.emplace_back([](TestContext& ctx) {
                assert(!ProxyFileSource::isTrackingActive());
                ProxyFileSource::setTrackingActive(true);
                ctx.getMetadata().metrics.network.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(networkProbeOp + mark),
                    std::forward_as_tuple(ProxyFileSource::getRequestCount(), ProxyFileSource::getTransferredSize()));
                return true;
            });
            continue;
        }
        result.emplace_back(unsupportedOperation(probe));
    }
    return result;
}

TestOperations getAfterOperations(const Manifest& manifest) {
    static const std::string mark = " - default - end";
    TestOperations result;
    for (const std::string& probe : manifest.getProbes()) {
        if (memoryProbeOp == probe) {
            result.emplace_back([](TestContext& ctx) {
                assert(AllocationIndex::isActive());
                auto emplaced = ctx.getMetadata().metrics.memory.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(memoryProbeOp + mark),
                    std::forward_as_tuple(AllocationIndex::getAllocatedSizePeak(),
                                          AllocationIndex::getAllocationsCount()));
                assert(emplaced.second);
                // TODO: Improve tolerance handling for memory tests.
                emplaced.first->second.tolerance = 0.2f;
                AllocationIndex::setActive(false);
                AllocationIndex::reset();
                return true;
            });
            continue;
        }
        if (gfxProbeOp == probe) {
            result.emplace_back([](TestContext& ctx) {
                // Compare memory allocations to the baseline probe
                GfxProbe metricProbe = ctx.activeGfxProbe;
                metricProbe.memIndexBuffers.peak -= ctx.baselineGfxProbe.memIndexBuffers.peak;
                metricProbe.memVertexBuffers.peak -= ctx.baselineGfxProbe.memVertexBuffers.peak;
                metricProbe.memTextures.peak -= ctx.baselineGfxProbe.memTextures.peak;
                ctx.getMetadata().metrics.gfx.insert({gfxProbeOp + mark, metricProbe});

                ctx.gfxProbeActive = false;
                return true;
            });
            continue;
        }
        if (networkProbeOp == probe) {
            result.emplace_back([](TestContext& ctx) {
                assert(ProxyFileSource::isTrackingActive());
                ctx.getMetadata().metrics.network.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(networkProbeOp + mark),
                    std::forward_as_tuple(ProxyFileSource::getRequestCount(), ProxyFileSource::getTransferredSize()));
                ProxyFileSource::setTrackingActive(false);
                return true;
            });
            continue;
        }
        result.emplace_back(unsupportedOperation(probe));
    }
    return result;
}

void resetContext(const TestMetadata& metadata, TestContext& ctx) {
    ctx.getFrontend().setSize(metadata.size);
    auto& map = ctx.getMap();
    map.setSize(metadata.size);
    map.setProjectionMode(mbgl::ProjectionMode()
                              .withAxonometric(metadata.axonometric)
                              .withXSkew(metadata.xSkew)
                              .withYSkew(metadata.ySkew));
    map.setDebug(metadata.debug);
    map.getStyle().loadJSON(serializeJsonValue(metadata.document));
    map.jumpTo(map.getStyle().getDefaultCamera());
}

} // namespace

TestRunner::Impl::Impl(const TestMetadata& metadata)
    : observer(std::make_unique<TestRunnerMapObserver>()),
      frontend(metadata.size, metadata.pixelRatio, swapBehavior(metadata.mapMode)),
      map(frontend,
          *observer.get(),
          mbgl::MapOptions()
              .withMapMode(metadata.mapMode)
              .withSize(metadata.size)
              .withPixelRatio(metadata.pixelRatio)
              .withCrossSourceCollisions(metadata.crossSourceCollisions),
          mbgl::ResourceOptions().withCacheOnlyRequestsSupport(false)) {}

TestRunner::Impl::~Impl() {}

bool TestRunner::run(TestMetadata& metadata) {
    AllocationIndex::setActive(false);
    AllocationIndex::reset();
    ProxyFileSource::setTrackingActive(false);

    struct ContextImpl final : public TestContext {
        ContextImpl(TestMetadata& metadata_) : metadata(metadata_) {}
        HeadlessFrontend& getFrontend() override {
            assert(runnerImpl);
            return runnerImpl->frontend;
        }
        Map& getMap() override {
            assert(runnerImpl);
            return runnerImpl->map;
        }
        TestRunnerMapObserver& getObserver() override {
            assert(runnerImpl);
            return *runnerImpl->observer;
        }
        TestMetadata& getMetadata() override { return metadata; }

        TestRunner::Impl* runnerImpl = nullptr;
        TestMetadata& metadata;
    };

    ContextImpl ctx(metadata);

    if (!metadata.ignoredTest) {
        for (const auto& operation : getBeforeOperations(manifest)) {
            if (!operation(ctx)) return false;
        }
    }

    std::string key = mbgl::util::toString(uint32_t(metadata.mapMode)) + "/" +
                      mbgl::util::toString(metadata.pixelRatio) + "/" +
                      mbgl::util::toString(uint32_t(metadata.crossSourceCollisions));

    if (maps.find(key) == maps.end()) {
        maps[key] = std::make_unique<TestRunner::Impl>(metadata);
    }

    ctx.runnerImpl = maps[key].get();
    auto& frontend = ctx.getFrontend();

    resetContext(metadata, ctx);

    for (const auto& operation : parseTestOperations(metadata, manifest)) {
        if (!operation(ctx)) return false;
    }

    HeadlessFrontend::RenderResult result;
    try {
        if (metadata.outputsImage) result = frontend.render(ctx.getMap());
    } catch (const std::exception&) {
        return false;
    }

    if (!metadata.ignoredTest) {
        ctx.activeGfxProbe = GfxProbe(result.stats, ctx.activeGfxProbe);
        for (const auto& operation : getAfterOperations(manifest)) {
            if (!operation(ctx)) return false;
        }
    }

    if (metadata.renderTest) {
        (void)checkProbingResults(metadata);
        return checkRenderTestResults(std::move(result.image), metadata);
    } else {
        std::vector<mbgl::Feature> features;
        assert(metadata.document["metadata"]["test"]["queryGeometry"].IsArray());
        if (metadata.document["metadata"]["test"]["queryGeometry"][0].IsNumber() &&
            metadata.document["metadata"]["test"]["queryGeometry"][1].IsNumber()) {
            features = frontend.getRenderer()->queryRenderedFeatures(metadata.queryGeometry, metadata.queryOptions);
        } else {
            features = frontend.getRenderer()->queryRenderedFeatures(metadata.queryGeometryBox, metadata.queryOptions);
        }
        return checkQueryTestResults(std::move(result.image), std::move(features), metadata);
    }
}

void TestRunner::reset() {
    maps.clear();
}
