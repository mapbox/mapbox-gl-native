#pragma once

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

class OnlineFileSource : public FileSource {
public:
    OnlineFileSource();
    ~OnlineFileSource() override;

    void setAPIBaseURL(const std::string& t) { apiBaseURL = t; }
    std::string getAPIBaseURL() const { return apiBaseURL; }

    void setAccessToken(const std::string& t) { accessToken = t; }
    std::string getAccessToken() const { return accessToken; }

    using ResourceTransform =
        std::function<std::unique_ptr<AsyncRequest>(Resource::Kind, std::string&&, std::function<void(std::string&&)>)>;
    void setResourceTransform(ResourceTransform&& cb);

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;

private:
    friend class OnlineFileRequest;

    class Impl;
    const std::unique_ptr<Impl> impl;
    std::string accessToken;
    std::string apiBaseURL = mbgl::util::API_BASE_URL;
};

} // namespace mbgl
