#ifndef MBGL_STORAGE_DEFAULT_ASSET_REQUEST
#define MBGL_STORAGE_DEFAULT_ASSET_REQUEST

#include "request_base.hpp"

namespace mbgl {

class AssetRequest : public RequestBase {
public:
    AssetRequest(const Resource&, Callback, const std::string& assetRoot);

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
