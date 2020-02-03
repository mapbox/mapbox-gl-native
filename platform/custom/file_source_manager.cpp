#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/main_resource_loader.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/async_request.hpp>
#include <mbgl/util/logging.hpp>

#define MBGL_TIMING
#include <mbgl/util/stopwatch.hpp>

namespace mbgl {

std::string toString(Resource::Kind kind) {
    switch (kind) {
        case Resource::Unknown:
            return "unknown";
        case Resource::Style:
            return "style";
        case Resource::Source:
            return "source";
        case Resource::Tile:
            return "tile";
        case Resource::Glyphs:
            return "glyphs";
        case Resource::SpriteImage:
            return "sprite-image";
        case Resource::SpriteJSON:
            return "sprite-json";
        case Resource::Image:
            return "image";
    }
    return "";
}

class TracingDelegate final : public FileSource {
public:
    TracingDelegate(std::shared_ptr<FileSource> fs_, std::string tag_) : fs(std::move(fs_)), tag(std::move(tag_)) {}
    ~TracingDelegate() = default;

    std::unique_ptr<AsyncRequest> request(const Resource& resource, Callback callback) override {
        Log::Debug(Event::Timing,
                   "%s request for '%s' of type '%s'",
                   tag.c_str(),
                   resource.url.c_str(),
                   toString(resource.kind).c_str());
        MBGL_TIMING_START(watch);
        return fs->request(resource, [=](Response response) {
            MBGL_TIMING_FINISH(watch,
                               tag << " response for '" << resource.url << "' of type '" << toString(resource.kind)
                                   << "' hasContent(" << (response.noContent ? "false)" : "true)") << " size "
                                   << (response.data != nullptr ? response.data->size() : 0) << "B Time");
            callback(response);
        });
    }

    bool canRequest(const Resource& res) const override { return fs->canRequest(res); }

private:
    std::shared_ptr<FileSource> fs;
    std::string tag;
};

class TracingFileSourceManagerImpl final : public FileSourceManager {
public:
    TracingFileSourceManagerImpl() {
        registerFileSourceFactory(FileSourceType::ResourceLoader, [](const ResourceOptions& options) {
            return std::make_unique<TracingDelegate>(std::make_unique<MainResourceLoader>(options),
                                                     "MainResourceLoader");
        });

        registerFileSourceFactory(FileSourceType::Asset, [](const ResourceOptions& options) {
            return std::make_unique<AssetFileSource>(options.assetPath());
        });

        registerFileSourceFactory(FileSourceType::Database, [](const ResourceOptions& options) {
            return std::make_unique<TracingDelegate>(std::make_unique<DatabaseFileSource>(options), "Database");
        });

        registerFileSourceFactory(FileSourceType::FileSystem,
                                  [](const ResourceOptions&) { return std::make_unique<LocalFileSource>(); });

        registerFileSourceFactory(FileSourceType::Network, [](const ResourceOptions& options) {
            auto networkSource = std::make_unique<OnlineFileSource>();
            networkSource->setProperty(ACCESS_TOKEN_KEY, options.accessToken());
            networkSource->setProperty(API_BASE_URL_KEY, options.baseURL());
            return std::make_unique<TracingDelegate>(std::move(networkSource), "Network");
        });
    }
};

FileSourceManager* FileSourceManager::get() noexcept {
    static TracingFileSourceManagerImpl instance;
    return &instance;
}

} // namespace mbgl
