#pragma once

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

class ResourceTransform;

// Properties that may be supported by online file sources.

// Property name to set / get an access token.
// type: std::string
constexpr const char* ACCESS_TOKEN_KEY = "access-token";

// Property name to set / get base url.
// type: std::string
constexpr const char* API_BASE_URL_KEY = "api-base-url";

// Property name to set / get maximum number of concurrent requests.
// type: unsigned
constexpr const char* MAX_CONCURRENT_REQUESTS_KEY = "max-concurrent-requests";

class OnlineFileSource : public FileSource {
public:
    OnlineFileSource();
    ~OnlineFileSource() override;

    // FileSource overrides
    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;
    bool canRequest(const Resource&) const override;
    void pause() override;
    void resume() override;
    void setProperty(const std::string&, const mapbox::base::Value&) override;
    mapbox::base::Value getProperty(const std::string&) const override;

    // OnlineFileSource interface.
    // TODO: Would be nice to drop it to get uniform interface.
    virtual void setResourceTransform(ResourceTransform);

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl
