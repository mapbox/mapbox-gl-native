#ifndef MBGL_STORAGE_ASSET_REQUEST
#define MBGL_STORAGE_ASSET_REQUEST

#include <mbgl/storage/base_request.hpp>

namespace mbgl {

typedef struct uv_loop_s uv_loop_t;

struct AssetRequestBaton;

class AssetRequest : public BaseRequest {
public:
    AssetRequest(const std::string &path, uv_loop_t *loop);
    ~AssetRequest();

    void cancel();

private:
    AssetRequestBaton *ptr = nullptr;

    friend struct AssetRequestBaton;
};

}

#endif
