#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/util.hpp"
#include "../fixtures/mock_file_source.hpp"
#include "../fixtures/mock_view.hpp"

#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/thread.hpp>

#include <regex>

using namespace mbgl;

namespace {

class MockMapContext : public Style::Observer {
public:
    MockMapContext(View& view,
                   FileSource& fileSource,
                   const std::function<void(std::exception_ptr error)>& callback)
        : data_(MapMode::Still, GLContextMode::Unique, view.getPixelRatio()),
          transform_(view),
          callback_(callback) {
        util::ThreadContext::setFileSource(&fileSource);

        transform_.resize({{ 1000, 1000 }});
        transform_.setLatLngZoom({0, 0}, 16);

        const std::string style = util::read_file("test/fixtures/resources/style.json");
        style_ = std::make_unique<Style>(data_);
        style_->setJSON(style, "");
        style_->setObserver(this);
    }

    ~MockMapContext() {
        cleanup();
    }

    void cleanup() {
        style_.reset();
    }

    void update() {
        const auto now = Clock::now();

        data_.setAnimationTime(now);
        transform_.updateTransitions(now);

        style_->update(transform_.getState(), texturePool_);
    }

    // Style::Observer implementation.
    void onTileDataChanged() override {
        update();

        if (style_->isLoaded()) {
            callback_(nullptr);
        }
    };

    void onResourceLoadingFailed(std::exception_ptr error) override {
        callback_(error);
    }

private:
    MapData data_;
    Transform transform_;
    TexturePool texturePool_;

    std::unique_ptr<Style> style_;

    std::function<void(std::exception_ptr error)> callback_;
};

void runTestCase(MockFileSource::Type type,
                 const std::string& param,
                 const std::string& message) {
    util::RunLoop loop(uv_default_loop());

    MockView view;
    MockFileSource fileSource(type, param);

    FixtureLogObserver* log = new FixtureLogObserver();
    Log::setObserver(std::unique_ptr<Log::Observer>(log));

    auto callback = [type, &loop, &param](std::exception_ptr error) {
        if (type == MockFileSource::Success) {
            EXPECT_TRUE(error == nullptr);
        } else {
            EXPECT_TRUE(error != nullptr);
        }

        try {
            if (error) {
                std::rethrow_exception(error);
            }
        } catch (const util::GlyphRangeLoadingException&) {
            EXPECT_EQ(param, "glyphs.pbf");
        } catch (const util::SourceLoadingException&) {
            EXPECT_TRUE(param == "source_raster.json" || param == "source_vector.json");
        } catch (const util::SpriteLoadingException&) {
            EXPECT_TRUE(param == "sprite.png" || param == "sprite.json");
        } catch (const util::TileLoadingException&) {
            EXPECT_TRUE(param == "raster.png" || param == "vector.pbf");
        } catch (const std::exception&) {
            EXPECT_TRUE(false) << "Unhandled exception.";
        }

        loop.stop();
    };

    std::unique_ptr<util::Thread<MockMapContext>> context(
        std::make_unique<util::Thread<MockMapContext>>(
            util::ThreadContext{"Map", util::ThreadType::Map, util::ThreadPriority::Regular}, view, fileSource, callback));

    uv_run(loop.get(), UV_RUN_DEFAULT);

    // Needed because it will make the Map thread
    // join and cease logging after this point.
    context->invoke(&MockMapContext::cleanup);
    context.reset();

    uint32_t match = 0;
    std::vector<FixtureLogObserver::LogMessage> logMessages = log->unchecked();

    for (auto& logMessage : logMessages) {
        if (std::regex_match(logMessage.msg, std::regex(message))) {
            match++;
        }
    }

    if (type == MockFileSource::Success) {
        EXPECT_EQ(match, 0u);
    } else {
        EXPECT_GT(match, 0u);
    }
}

}

class ResourceLoading : public ::testing::TestWithParam<std::pair<std::string, std::string>> {
};

TEST_P(ResourceLoading, Success) {
    runTestCase(MockFileSource::Success, GetParam().first, std::string());
}

TEST_P(ResourceLoading, RequestFail) {
    std::stringstream message;
    message << "Failed to load \\[test\\/fixtures\\/resources\\/" << GetParam().first << "\\]\\: Failed by the test case";

    runTestCase(MockFileSource::RequestFail, GetParam().first, message.str());
}

TEST_P(ResourceLoading, RequestWithCorruptedData) {
    runTestCase(MockFileSource::RequestWithCorruptedData, GetParam().first, GetParam().second);
}

INSTANTIATE_TEST_CASE_P(Style, ResourceLoading,
    ::testing::Values(
        std::make_pair("source_raster.json", "Failed to parse \\[test\\/fixtures\\/resources\\/source_raster.json\\]: 0 - Invalid value."),
        std::make_pair("source_vector.json", "Failed to parse \\[test\\/fixtures\\/resources\\/source_vector.json\\]: 0 - Invalid value."),
        std::make_pair("sprite.json", "Failed to parse JSON: Invalid value. at offset 0"),
        std::make_pair("sprite.png", "Could not parse sprite image"),
        std::make_pair("raster.png", "Failed to parse \\[17\\/6553(4|5|6|7)\\/6553(4|5|6|7)\\]\\: error parsing raster image"),
        std::make_pair("vector.pbf", "Failed to parse \\[1(5|6)\\/1638(3|4)\\/1638(3|4)\\]\\: pbf unknown field type exception"),
        std::make_pair("glyphs.pbf", "Failed to parse \\[test\\/fixtures\\/resources\\/glyphs.pbf\\]: pbf unknown field type exception")));
