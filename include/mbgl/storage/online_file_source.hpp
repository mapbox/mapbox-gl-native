#pragma once

#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {

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
    void setResourceTransform(ResourceTransform) override;

private:
    class Impl;
    const std::unique_ptr<Impl> impl;
};

} // namespace mbgl
