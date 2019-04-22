#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/storage/resource.hpp>

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/async_request.hpp>
#include <mbgl/util/optional.hpp>

#include <functional>
#include <memory>

namespace mbgl {

class ResourceOptions;
class ResourceTransform;

class FileSource : private util::noncopyable {
public:
    virtual ~FileSource() = default;

    using Callback = std::function<void (Response)>;

    // Request a resource. The callback will be called asynchronously, in the same
    // thread as the request was made. This thread must have an active RunLoop. The
    // request may be cancelled before completion by releasing the returned AsyncRequest.
    // If the request is cancelled before the callback is executed, the callback will
    // not be executed.
    virtual std::unique_ptr<AsyncRequest> request(const Resource&, Callback) = 0;

    // When a file source supports consulting a local cache only, it must return true.
    // Cache-only requests are requests that aren't as urgent, but could be useful, e.g.
    // to cover part of the map while loading. The FileSource should only do cheap actions to
    // retrieve the data, e.g. load it from a cache, but not from the internet.
    virtual bool supportsCacheOnlyRequests() const {
        return false;
    }

    // Factory for creating a platform-specific file source.
    static std::shared_ptr<FileSource> createPlatformFileSource(const ResourceOptions&);

    // Singleton for obtaining the shared platform-specific file source. A single instance of a file source is provided
    // for each unique combination of a Mapbox API base URL, access token, cache path and platform context.
    static std::shared_ptr<FileSource> getSharedFileSource(const ResourceOptions&);
};

} // namespace mbgl
