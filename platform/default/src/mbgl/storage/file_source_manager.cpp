#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/local_file_source.hpp>
#include <mbgl/storage/main_resource_loader.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/resource_options.hpp>

namespace mbgl {

class DefaultFileSourceManagerImpl final : public FileSourceManager {
public:
    DefaultFileSourceManagerImpl() {
        registerFileSourceFactory(FileSourceType::ResourceLoader, [](const ResourceOptions& options) {
            return std::make_unique<MainResourceLoader>(options);
        });

        registerFileSourceFactory(FileSourceType::Asset, [](const ResourceOptions& options) {
            return std::make_unique<AssetFileSource>(options.assetPath());
        });

        registerFileSourceFactory(FileSourceType::Database, [](const ResourceOptions& options) {
            return std::make_unique<DatabaseFileSource>(options);
        });

        registerFileSourceFactory(FileSourceType::FileSystem,
                                  [](const ResourceOptions&) { return std::make_unique<LocalFileSource>(); });

        registerFileSourceFactory(FileSourceType::Network, [](const ResourceOptions& options) {
            auto networkSource = std::make_unique<OnlineFileSource>();
            networkSource->setProperty(ACCESS_TOKEN_KEY, options.accessToken());
            networkSource->setProperty(API_BASE_URL_KEY, options.baseURL());
            return networkSource;
        });
    }
};

FileSourceManager* FileSourceManager::get() noexcept {
    static DefaultFileSourceManagerImpl instance;
    return &instance;
}

} // namespace mbgl
