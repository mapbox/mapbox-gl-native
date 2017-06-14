#pragma once

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

namespace util {
template <typename T> class ThreadedObject;
} // namespace util

class AssetFileSource : public FileSource {
public:
    AssetFileSource(const std::string& assetRoot);
    ~AssetFileSource() override;

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;

private:
    class Impl;

    std::unique_ptr<util::ThreadedObject<Impl>> impl;
};

} // namespace mbgl
