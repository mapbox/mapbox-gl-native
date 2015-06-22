#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/util.hpp"
#include "mock_file_source.hpp"
#include "mock_view.hpp"

#include <mbgl/map/environment.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/transform_state.hpp>
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
    MockMapContext(uv_loop_t* loop,
                   View& view,
                   FileSource& fileSource,
                   const std::function<void(std::exception_ptr error)>& callback)
        : env_(fileSource),
          envScope_(env_, ThreadType::Map, "Map"),
          data_(view, MapMode::Still),
          callback_(callback) {

        data_.transform.resize(1000, 1000, 1.0, 1000, 1000);
        data_.transform.setLatLngZoom({0, 0}, 16);

        const std::string style = util::read_file("test/fixtures/resources/style.json");
        style_ = std::make_unique<Style>(style, "", loop, env_),
        style_->setObserver(this);
    }

    ~MockMapContext() {
        style_.reset();
    }

    void update() {
        const auto now = Clock::now();

        data_.setAnimationTime(now);
        data_.transform.updateTransitions(now);

        transformState_ = data_.transform.currentState();
        style_->update(data_, transformState_, texturePool_);
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
    Environment env_;
    EnvironmentScope envScope_;

    MapData data_;
    TransformState transformState_;
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
            EXPECT_EQ(param, "source.json");
        } catch (const util::SpriteLoadingException&) {
            EXPECT_TRUE(param == "sprite.png" || param == "sprite.json");
        } catch (const util::TileLoadingException&) {
            EXPECT_EQ(param, "vector.pbf");
        } catch (const std::exception&) {
            EXPECT_TRUE(false) << "Unhandled exception.";
        }

        loop.stop();
    };

    std::unique_ptr<util::Thread<MockMapContext>> context(
        std::make_unique<util::Thread<MockMapContext>>(
            "Map", util::ThreadPriority::Regular, view, fileSource, callback));

    uv_run(loop.get(), UV_RUN_DEFAULT);

    // Needed because it will make the Map thread
    // join and cease logging after this point.
    context.reset();

    uint32_t match = 0;
    std::vector<FixtureLogObserver::LogMessage> logMessages = log->unchecked();

    for (auto& logMessage : logMessages) {
        if (std::regex_match(*logMessage.msg, std::regex(message))) {
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

class ResourceLoading : public ::testing::TestWithParam<std::string> {
};

TEST_P(ResourceLoading, Success) {
    runTestCase(MockFileSource::Success, GetParam(), std::string());
}

TEST_P(ResourceLoading, RequestFail) {
    std::stringstream message;
    message << "Failed to load \\[test/fixtures/resources/" << GetParam() << "]: Failed by the test case";

    runTestCase(MockFileSource::RequestFail, GetParam(), message.str());
}

TEST_P(ResourceLoading, RequestWithCorruptedData) {
    const std::string param(GetParam());

    std::stringstream message;
    message << "Failed to parse ";

    if (param == "vector.pbf") {
        message << "\\[15/1638./1638.]: pbf unknown field type exception";
    } else {
        message << "\\[test/fixtures/resources/" << param << "]";
    }

    if (param.find("json") != std::string::npos) {
        message << ": 0 - Expect either an object or array at root";
    }

    runTestCase(MockFileSource::RequestWithCorruptedData, GetParam(), message.str());
}

INSTANTIATE_TEST_CASE_P(Style, ResourceLoading,
    ::testing::Values("source.json", "sprite.json", "sprite.png", "vector.pbf", "glyphs.pbf"));
