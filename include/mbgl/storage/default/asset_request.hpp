#ifndef MBGL_STORAGE_DEFAULT_ASSET_REQUEST
#define MBGL_STORAGE_DEFAULT_ASSET_REQUEST

#include "shared_request_base.hpp"

namespace mbgl {

class AssetRequest : public SharedRequestBase {
public:
    AssetRequest(DefaultFileSource *source, const Resource &resource);

    void start(uv_loop_t *loop, std::unique_ptr<Response> response = nullptr);
    void cancel();

private:
    ~AssetRequest();
    void *ptr = nullptr;

    friend class AssetRequestImpl;
};

}

#endif
