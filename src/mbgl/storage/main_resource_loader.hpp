#pragma once

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

class ResourceTransform;
class ResourceOptions;

class MainResourceLoader final : public FileSource {
public:
    explicit MainResourceLoader(const ResourceOptions& options);
    ~MainResourceLoader() override;

    bool supportsCacheOnlyRequests() const override;
    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;
    bool canRequest(const Resource&) const override;
    void pause() override;
    void resume() override;

private:
    friend class ResourceLoaderRequestor;
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl
