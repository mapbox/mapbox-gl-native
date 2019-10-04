#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/storage/resource.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/util/async_request.hpp>
#include <mbgl/util/logging.hpp>

#include <atomic>

#include "file_source.hpp"

namespace mbgl {

std::atomic_size_t requestCount{0};
std::atomic_size_t transferredSize{0};
std::atomic_bool active{false};
std::atomic_bool offline{true};

ProxyFileSource::ProxyFileSource(std::shared_ptr<FileSource> defaultResourceLoader_, const ResourceOptions& options)
    : defaultResourceLoader(std::move(defaultResourceLoader_)) {
    assert(defaultResourceLoader);
    if (offline) {
        auto dbfs = FileSourceManager::get()->getFileSource(FileSourceType::Database, options);
        dbfs->setProperty("read-only-mode", true);
    }
}

ProxyFileSource::~ProxyFileSource() = default;

std::unique_ptr<AsyncRequest> ProxyFileSource::request(const Resource& resource, Callback callback) {
    auto transformed = resource;

    // If offline, force always loading the resource from the cache
    // so we don't make any network request.
    if (offline) {
        transformed.loadingMethod = Resource::LoadingMethod::CacheOnly;
    }

    // This is needed for compatibility with the style tests that
    // are using local:// instead of http:// which is the schema
    // we support for cached files.
    if (transformed.url.compare(0, 8, "local://") == 0) {
        transformed.url.replace(0, 8, "http://");

        if (transformed.kind == Resource::Kind::Tile && transformed.tileData) {
            transformed.tileData->urlTemplate.replace(0, 8, "http://");
        }
    }

    if (transformed.url.compare(0, 22, "http://localhost:2900/") == 0) {
        transformed.url.replace(0, 22, "http://");

        if (transformed.kind == Resource::Kind::Tile && transformed.tileData) {
            transformed.tileData->urlTemplate.replace(0, 22, "http://");
        }
    }

    return defaultResourceLoader->request(transformed, [=](Response response) {
        if (transformed.loadingMethod == Resource::LoadingMethod::CacheOnly && response.noContent) {
            if (transformed.kind == Resource::Kind::Tile && transformed.tileData) {
                mbgl::Log::Info(mbgl::Event::Database,
                                "Resource not found in cache: %s (%s)",
                                transformed.url.c_str(),
                                transformed.tileData->urlTemplate.c_str());
            } else {
                mbgl::Log::Info(mbgl::Event::Database, "Resource not found in cache: %s", transformed.url.c_str());
            }
        }

        std::size_t size = response.data != nullptr ? response.data->size() : 0;
        if (active) {
            requestCount++;
            transferredSize += size;
        }
        callback(response);
    });
}

// static
void ProxyFileSource::setOffline(bool status) {
    offline = status;
}

// static
void ProxyFileSource::setTrackingActive(bool active_) {
    active = active_;
    requestCount = 0;
    transferredSize = 0;
}

// static
bool ProxyFileSource::isTrackingActive() {
    return active;
}

// static
size_t ProxyFileSource::getRequestCount() {
    return requestCount;
}

// static
size_t ProxyFileSource::getTransferredSize() {
    return transferredSize;
}

} // namespace mbgl
