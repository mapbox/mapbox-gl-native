#pragma once

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

class HTTPFileSource : public FileSource {
public:
    HTTPFileSource();
    ~HTTPFileSource() override;

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;

    static uint32_t maximumConcurrentRequests();

    class Impl;

private:
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
