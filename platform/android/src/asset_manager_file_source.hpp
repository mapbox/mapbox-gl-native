#pragma once

#include <mbgl/storage/file_source.hpp>

#include "asset_manager.hpp"

#include <jni/jni.hpp>

namespace mbgl {

namespace util {
template <typename T> class Thread;
} // namespace util

class AssetManagerFileSource : public FileSource {
public:
    AssetManagerFileSource(jni::JNIEnv&, jni::Object<android::AssetManager>);
    ~AssetManagerFileSource() override;

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;

private:
    class Impl;

    jni::UniqueObject<android::AssetManager> assetManager;
    std::unique_ptr<util::Thread<Impl>> impl;
};

} // namespace mbgl
