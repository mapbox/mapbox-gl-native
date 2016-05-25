#pragma once

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

class OnlineFileSource : public FileSource {
public:
    OnlineFileSource();
    ~OnlineFileSource() override;

    void setAccessToken(const std::string& t) { accessToken = t; }
    std::string getAccessToken() const { return accessToken; }

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;

private:
    friend class OnlineFileRequest;

    class Impl;
    const std::unique_ptr<Impl> impl;
    std::string accessToken;
};

} // namespace mbgl
