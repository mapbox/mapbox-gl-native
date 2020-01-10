#pragma once

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class AssetFileSource : public FileSource {
public:
    AssetFileSource(const std::string& assetPath);
    ~AssetFileSource() override;

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;
    bool canRequest(const Resource&) const override;
    void pause() override;
    void resume() override;

private:
    class Impl;

    std::unique_ptr<util::Thread<Impl>> impl;
};

} // namespace mbgl
