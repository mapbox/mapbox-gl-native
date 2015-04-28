#ifndef MBGL_STORAGE_DEFAULT_ASSET_REQUEST
#define MBGL_STORAGE_DEFAULT_ASSET_REQUEST

#include "request_base.hpp"

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class AssetRequestImpl;

class AssetRequest : public RequestBase {
public:
    AssetRequest(const Resource&, Callback, uv_loop_t*, const std::string& assetRoot);

    void cancel() override;

private:
    ~AssetRequest();

    AssetRequestImpl* impl;
    friend class AssetRequestImpl;
};

}

#endif
