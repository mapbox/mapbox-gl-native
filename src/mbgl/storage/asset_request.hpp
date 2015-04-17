#ifndef MBGL_STORAGE_DEFAULT_ASSET_REQUEST
#define MBGL_STORAGE_DEFAULT_ASSET_REQUEST

#include "shared_request_base.hpp"

namespace mbgl {

class AssetRequest : public SharedRequestBase {
public:
    AssetRequest(DefaultFileSource::Impl&, const Resource&, const std::string& assetRoot);

    void start(uv_loop_t *loop, std::shared_ptr<const Response> response = nullptr);
    void cancel();

private:
    ~AssetRequest();
    void *ptr = nullptr;
    const std::string& assetRoot;

    friend class AssetRequestImpl;
};

}

#endif
