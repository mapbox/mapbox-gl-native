#pragma once

#include <mbgl/storage/response.hpp>

#include <mapbox/value.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class AsyncRequest;
class Resource;

// TODO: Rename to ResourceProviderType
enum FileSourceType : uint8_t {
    Asset,
    // TODO: split to separate types
    // - Cache for fast KV store (FASTER, LevelDB, RocksDB)
    // - Database for read-only offline use-cases
    Database,
    FileSystem,
    Network,
    // Resource loader acts as a proxy and has logic
    // for request delegation to Asset, Cache, and other
    // file sources.
    ResourceLoader
};

// TODO: Rename to ResourceProvider to avoid confusion with
// GeoJSONSource, RasterSource, VectorSource, CustomGeometrySource and other *Sources.
class FileSource {
public:
    FileSource(const FileSource&) = delete;
    FileSource& operator=(const FileSource&) = delete;
    virtual ~FileSource() = default;

    using Callback = std::function<void (Response)>;

    // Request a resource. The callback will be called asynchronously, in the same
    // thread as the request was made. This thread must have an active RunLoop. The
    // request may be cancelled before completion by releasing the returned AsyncRequest.
    // If the request is cancelled before the callback is executed, the callback will
    // not be executed.
    virtual std::unique_ptr<AsyncRequest> request(const Resource&, Callback) = 0;

    // Allows to forward response from one source to another.
    // Optionally, callback can be provided to receive notification for forward
    // operation.
    virtual void forward(const Resource&, const Response&, std::function<void()> = {}) {}

    // When a file source supports consulting a local cache only, it must return true.
    // Cache-only requests are requests that aren't as urgent, but could be useful, e.g.
    // to cover part of the map while loading. The FileSource should only do cheap actions to
    // retrieve the data, e.g. load it from a cache, but not from the internet.
    virtual bool supportsCacheOnlyRequests() const { return false; }

    // Checks whether a resource could be requested from this file source.
    virtual bool canRequest(const Resource&) const = 0;

    /*
     * Pause file request activity.
     *
     * If pause is called then no revalidation or network request activity
     * will occur.
     */
    virtual void pause() {}

    /*
     * Resume file request activity.
     *
     * Calling resume will unpause the file source and process any tasks that
     * expired while the file source was paused.
     */
    virtual void resume() {}

    /*
     * Generic property setter / getter methods.
     */
    virtual void setProperty(const std::string&, const mapbox::base::Value&){};
    virtual mapbox::base::Value getProperty(const std::string&) const { return {}; };

protected:
    FileSource() = default;
};

} // namespace mbgl
