#include "../fixtures/fixture_log_observer.hpp"
#include "../fixtures/util.hpp"
#include "mock_file_source.hpp"
#include "mock_view.hpp"

#include <mbgl/geometry/glyph_atlas.hpp>
#include <mbgl/geometry/sprite_atlas.hpp>
#include <mbgl/map/environment.hpp>
#include <mbgl/map/map_data.hpp>
#include <mbgl/map/resource_loader.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/texture_pool.hpp>
#include <mbgl/util/thread.hpp>

using namespace mbgl;

namespace {

class MockMapContext : public ResourceLoader::Observer {
public:
    MockMapContext(uv_loop_t* loop,
                   View& view,
                   FileSource& fileSource,
                   const std::function<void(std::exception_ptr error)>& callback)
        : env_(fileSource),
          envScope_(env_, ThreadType::Map, "Map"),
          data_(view, MapMode::Still),
          glyphStore_(std::make_unique<GlyphStore>(loop, env_)),
          glyphAtlas_(std::make_unique<GlyphAtlas>(1024, 1024)),
          spriteAtlas_(std::make_unique<SpriteAtlas>(512, 512)),
          texturePool_(std::make_unique<TexturePool>()),
          style_(std::make_unique<Style>()),
          resourceLoader_(std::make_unique<ResourceLoader>()),
          asyncUpdate(std::make_unique<uv::async>(loop, [this] { update(); })),
          callback_(callback) {
        asyncUpdate->unref();

        data_.transform.resize(1000, 1000, 1.0, 1000, 1000);
        data_.transform.setLatLngZoom({0, 0}, 16);

        const std::string style = util::read_file("test/fixtures/resources/style.json");
        style_->loadJSON(reinterpret_cast<const uint8_t *>(style.c_str()));

        glyphStore_->setURL(style_->glyph_url);

        resourceLoader_->setGlyphStore(glyphStore_.get());
        resourceLoader_->setObserver(this);
        resourceLoader_->setStyle(style_.get());
    }

    ~MockMapContext() {
        resourceLoader_.reset();
        style_.reset();
        texturePool_.reset();
        spriteAtlas_.reset();
        glyphAtlas_.reset();
        glyphStore_.reset();
    }

    void update() {
        const auto now = Clock::now();

        data_.setAnimationTime(now);
        data_.transform.updateTransitions(now);

        transformState_ = data_.transform.currentState();

        resourceLoader_->update(
            data_, transformState_, *glyphAtlas_, *spriteAtlas_, *texturePool_);
    }

    // ResourceLoader::Observer implementation.
    void onTileDataChanged() override {
        util::ptr<Sprite> sprite = resourceLoader_->getSprite();
        if (sprite && sprite->isLoaded() && style_->isLoaded()) {
            callback_(nullptr);
        }

        asyncUpdate->send();
    };

    void onResourceLoadingFailed(std::exception_ptr error) override {
        callback_(error);
    }

private:
    Environment env_;
    EnvironmentScope envScope_;

    MapData data_;
    TransformState transformState_;

    std::unique_ptr<GlyphStore> glyphStore_;
    std::unique_ptr<GlyphAtlas> glyphAtlas_;
    std::unique_ptr<SpriteAtlas> spriteAtlas_;
    std::unique_ptr<TexturePool> texturePool_;
    std::unique_ptr<Style> style_;
    std::unique_ptr<ResourceLoader> resourceLoader_;

    std::unique_ptr<uv::async> asyncUpdate;

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

    auto callback = [&loop, &param](std::exception_ptr error) {
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

    const FixtureLogObserver::LogMessage logMessage {
        EventSeverity::Error,
        Event::ResourceLoader,
        int64_t(-1),
        message,
    };

    if (type == MockFileSource::Success) {
        EXPECT_EQ(log->count(logMessage), 0u);
    } else {
        EXPECT_GT(log->count(logMessage), 0u);
    }

    // Clear the remaining error messages
    log->unchecked().size();
}

}

class ResourceLoaderTest : public ::testing::TestWithParam<std::string> {
};

TEST_P(ResourceLoaderTest, Success) {
    runTestCase(MockFileSource::Success, GetParam(), std::string());
}

TEST_P(ResourceLoaderTest, RequestFail) {
    std::stringstream message;
    message << "Failed to load [test/fixtures/resources/" << GetParam() << "]: Failed by the test case";

    runTestCase(MockFileSource::RequestFail, GetParam(), message.str());
}

TEST_P(ResourceLoaderTest, RequestWithCorruptedData) {
    const std::string param(GetParam());

    std::stringstream message;
    message << "Failed to parse ";

    if (param == "vector.pbf") {
        message << "[15/16384/16384]: pbf unknown field type exception";
    } else {
        message << "[test/fixtures/resources/" << param << "]";
    }

    if (param.find("json") != std::string::npos) {
        message << ": 0 - Expect either an object or array at root";
    }

    runTestCase(MockFileSource::RequestWithCorruptedData, GetParam(), message.str());
}

INSTANTIATE_TEST_CASE_P(ResourceLoader, ResourceLoaderTest,
    ::testing::Values("source.json", "sprite.json", "sprite.png", "vector.pbf", "glyphs.pbf"));
