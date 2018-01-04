#include "asset_manager_file_source.hpp"

#include <mbgl/storage/file_source_request.hpp>
#include <mbgl/storage/response.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/url.hpp>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace mbgl {

class AssetManagerFileSource::Impl {
public:
    Impl(ActorRef<Impl>, AAssetManager* assetManager_) : assetManager(assetManager_) {
    }

    void request(const std::string& url, ActorRef<FileSourceRequest> req) {
        // Note: AssetManager already prepends "assets" to the filename.
        const std::string path = mbgl::util::percentDecode(url.substr(8));

        Response response;

        if (AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_BUFFER)) {
            response.data = std::make_shared<std::string>(
                reinterpret_cast<const char*>(AAsset_getBuffer(asset)), AAsset_getLength64(asset));
            AAsset_close(asset);
        } else {
            response.error = std::make_unique<Response::Error>(Response::Error::Reason::NotFound,
                                                               "Could not read asset");
        }

        req.invoke(&FileSourceRequest::setResponse, response);
    }

private:
    AAssetManager* assetManager;
};

AssetManagerFileSource::AssetManagerFileSource(jni::JNIEnv& env, jni::Object<android::AssetManager> assetManager_)
    : assetManager(assetManager_.NewGlobalRef(env)),
      impl(std::make_unique<util::Thread<Impl>>("AssetManagerFileSource",
          AAssetManager_fromJava(&env, jni::Unwrap(**assetManager)))) {
}

AssetManagerFileSource::~AssetManagerFileSource() = default;

std::unique_ptr<AsyncRequest> AssetManagerFileSource::request(const Resource& resource, Callback callback) {
    auto req = std::make_unique<FileSourceRequest>(std::move(callback));

    impl->actor().invoke(&Impl::request, resource.url, req->actor());

    return std::move(req);
}

} // namespace mbgl
